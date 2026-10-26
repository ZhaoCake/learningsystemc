#!/usr/bin/env python3
"""learningsystemc runner — rustlings-style checker for SystemC exercises.

用法（在 learningsystemc/ 目录下）：
  python lsx.py list          列出关卡与进度
  python lsx.py next          跑下一关
  python lsx.py run ex07      跑指定关卡（也可只写 7）
  python lsx.py hint ex07     看提示
  python lsx.py reset ex07    恢复初始代码
  python lsx.py verify        编译并运行 solutions/ 全部参考解答（维护用）

默认使用仓库自带的 SystemC（third_party/systemc-install）+ MSVC。
可用环境变量覆盖：
  SYSTEMC_HOME   SystemC 安装前缀（含 include/systemc.h 与 lib/systemc.lib）
  LSC_CXX        编译器；设为 g++ 时走 GCC 路径
"""
import glob, json, os, re, shutil, subprocess, sys, tempfile

ROOT = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(ROOT)
EX_DIR = os.path.join(ROOT, 'exercises')
TEST_DIR = os.path.join(ROOT, 'tests')
SOL_DIR = os.path.join(ROOT, 'solutions')
BUILD_DIR = os.path.join(ROOT, 'build')
PROGRESS = os.path.join(ROOT, '.lsx_progress.json')
SYSTEMC_ROOT_LOCAL = os.path.join(REPO, 'third_party', 'systemc-install')

# MSVC 用 /std:c++17，GCC 用 -std=gnu++14（SystemC 2.3.x 源码树的习惯写法）
STD_GNU = os.environ.get('LSC_STD', 'gnu++14')

ANSI = dict(g='\033[32m', r='\033[31m', y='\033[33m', b='\033[36m', dim='\033[2m', reset='\033[0m')


def _enable_ansi():
    """Windows 10+ 控制台默认不开 VT 序列，这里手动打开，否则颜色码原样打印。"""
    if os.name != 'nt':
        return
    try:
        import ctypes
        k32 = ctypes.windll.kernel32
        k32.SetConsoleMode(k32.GetStdHandle(-11), 7)
    except Exception:
        pass


def dec(b):
    """子进程输出解码：优先 UTF-8，退回中文 Windows 的 GBK，最后带替换字符。"""
    for enc in ('utf-8', 'gbk'):
        try:
            return b.decode(enc)
        except UnicodeDecodeError:
            pass
    return b.decode('utf-8', 'replace')


def c(code, s):
    return f"{code}{s}{ANSI['reset']}" if sys.stdout.isatty() else s


# ---------- environment ----------
def find_systemc():
    """返回 (include_dir, lib_dir)；lib 里应有 systemc.lib / libsystemc.a。"""
    cand = [os.environ.get('SYSTEMC_HOME', ''), SYSTEMC_ROOT_LOCAL,
            os.path.join(REPO, 'third_party', 'systemc-install-debug')]
    cand += sorted(glob.glob('/opt/systemc-*') + glob.glob('/usr/local/systemc-*')
                   + glob.glob(os.path.expanduser('~/.local/systemc-*')))
    for p in cand:
        if not p:
            continue
        inc = os.path.join(p, 'include')
        if not os.path.isfile(os.path.join(inc, 'systemc.h')):
            continue
        for libname in ('lib', 'lib64', 'lib-linux64', 'lib-win64'):
            lib = os.path.join(p, libname)
            if os.path.isdir(lib):
                return inc, lib
        return inc, p
    # 某些发行版把 SYSTEMC 直接装进 /usr
    if os.path.exists('/usr/include/systemc.h'):
        return '/usr/include', '/usr/lib'
    return None, None


def find_vcvars():
    """用 vswhere 定位 VS 的 vcvars64.bat；找不到返回 None。"""
    pf86 = os.environ.get('ProgramFiles(x86)', r'C:\Program Files (x86)')
    vswhere = os.path.join(pf86, 'Microsoft Visual Studio', 'Installer', 'vswhere.exe')
    if not os.path.isfile(vswhere):
        return None
    try:
        r = subprocess.run([vswhere, '-latest', '-products', '*',
                            '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
                            '-property', 'installationPath'],
                           capture_output=True, timeout=60)
    except Exception:
        return None
    lines = dec(r.stdout).strip().splitlines()
    if not lines:
        return None
    bat = os.path.join(lines[0].strip(), 'VC', 'Auxiliary', 'Build', 'vcvars64.bat')
    return bat if os.path.isfile(bat) else None


def vcvars_env(vcvars):
    """跑一次 vcvars64.bat 并抓下它设置的环境变量（INCLUDE/LIB/PATH/...）。

    走临时 .bat 是因为 `cmd /c call "路径带空格" && set` 在参数列表里
    的引号会被转义坏掉；写成文件最稳。抓完缓存到进程内，后续编译直接调 cl.exe。
    """
    bat = os.path.join(tempfile.gettempdir(), 'lsc_vcvars.bat')
    with open(bat, 'w', encoding='ascii', errors='replace') as f:
        f.write('@echo off\r\ncall "%s" >nul\r\nset\r\n' % vcvars)
    try:
        r = subprocess.run(['cmd', '/c', bat], capture_output=True, timeout=180)
        out = dec(r.stdout)
    finally:
        try:
            os.remove(bat)
        except OSError:
            pass
    env = dict(os.environ)
    for line in out.splitlines():
        if '=' in line:
            k, v = line.split('=', 1)
            env[k] = v
    return env


_TOOLCHAIN = None


def toolchain():
    """('msvc', env, cl_path) 或 ('gnu', env, cxx)。首次调用时探测并缓存。"""
    global _TOOLCHAIN
    if _TOOLCHAIN:
        return _TOOLCHAIN
    forced = os.environ.get('LSC_CXX') or os.environ.get('CXX') or ''
    if os.name == 'nt' and 'cl' not in os.path.basename(forced).lower():
        vcvars = find_vcvars()
        if vcvars:
            env = vcvars_env(vcvars)
            cl = os.path.join(env.get('VCToolsInstallDir', ''),
                              'bin', 'Hostx64', 'x64', 'cl.exe')
            if not os.path.isfile(cl):
                cl = shutil.which('cl', path=env.get('PATH', '')) or 'cl'
            _TOOLCHAIN = ('msvc', env, cl)
            return _TOOLCHAIN
    _TOOLCHAIN = ('gnu', dict(os.environ), forced or 'g++')
    return _TOOLCHAIN


def load_progress():
    if os.path.exists(PROGRESS):
        return json.load(open(PROGRESS))
    return {'done': []}


def save_progress(p):
    json.dump(p, open(PROGRESS, 'w'), indent=1)


# ---------- exercises ----------
def ex_id(path):
    m = re.match(r'ex(\d+)_.*\.cpp$', os.path.basename(path))
    return int(m.group(1)) if m else None


def all_exercises():
    exs = sorted(glob.glob(os.path.join(EX_DIR, 'ex*.cpp')))
    return [(ex_id(e), e) for e in exs if ex_id(e) is not None]


def find_ex(q):
    """Accept 'ex07', '7', or full name."""
    exs = all_exercises()
    q = str(q).lower().strip()
    if q.startswith('ex'):
        q = q[2:]
    try:
        want = int(q)          # 'ex07' / '7' / '07' 都认
    except ValueError:
        want = None
    for i, p in exs:
        if i == want:
            return i, p
    return None, None


def next_exercise():
    prog = load_progress()
    for i, p in all_exercises():
        if i not in prog['done']:
            return i, p
    return None, None


# ---------- build & run ----------
def compile_ex(src, out):
    """编译单个练习。返回 subprocess 结果（stderr 里是编译器输出）。"""
    inc, lib = find_systemc()
    if not inc:
        print(c(ANSI['r'], '找不到 SystemC。请设置 SYSTEMC_HOME 指向 SystemC 安装前缀'
                           '（含 include/systemc.h 与 lib/systemc.lib）'))
        sys.exit(2)
    kind, env, cc = toolchain()
    os.makedirs(BUILD_DIR, exist_ok=True)
    if kind == 'msvc':
        # /vmg 必须有：SystemC 的 SC_METHOD/SC_THREAD 宏要把成员函数指针
        # 转换成统一表示，MSVC 默认的“最紧凑”表示会让这种转换在运行期崩掉
        # （C4407 警告 + 一进 sc_start 就 0xC0000005）。CMake 工程里也是这么传的。
        cmd = [cc, '/nologo', '/std:c++17', '/EHsc', '/MD', '/vmg', '/utf-8', '/W3', '/Od',
               f'/I{inc}', os.path.abspath(src),
               f'/Fe:{os.path.abspath(out)}', f'/Fo:{BUILD_DIR}\\',
               '/link', f'/LIBPATH:{lib}', 'systemc.lib', '/SUBSYSTEM:CONSOLE']
    else:
        cmd = [cc, f'-std={STD_GNU}', '-Wall', '-O0', '-g', f'-I{inc}',
               '-o', out, src, f'-L{lib}', '-lsystemc', '-lm',
               f'-Wl,-rpath,{lib}']
    try:
        r = subprocess.run(cmd, capture_output=True, timeout=300, cwd=BUILD_DIR, env=env)
    except FileNotFoundError:
        print(c(ANSI['r'], f'找不到编译器 {cc}；用 LSC_CXX 指定一个'))
        sys.exit(2)
    r.stdout, r.stderr = dec(r.stdout), dec(r.stderr)
    return r


BANNER_PAT = [
    re.compile(r'^(Info|Error|Warning|Fatal):\s*$'),
    re.compile(r'SystemC 2\.\d+\.\d+.*---'),          # 含编译日期，非确定
    re.compile(r'^\s*Copyright \(c\) 1996-20\d\d'),
    re.compile(r'^\s*(Info|Warning): \(I\d+\)'),       # 内核提示，非确定
    re.compile(r'^\s*ALL RIGHTS RESERVED'),
]


def normalize(s):
    # 丢弃空行：SystemC 默认报告 handler 会在输出里夹前导换行，
    # 练习输出本身不依赖空行，统一去掉以保证 diff 稳定
    lines = [l.rstrip() for l in s.replace('\r\n', '\n').split('\n')]
    lines = [l for l in lines if l.strip() and not any(p.search(l) for p in BANNER_PAT)]
    return '\n'.join(lines)


def run_ex(num, path, verbose=False):
    name = os.path.basename(path)
    exe = 'ex%02d.exe' % num if os.name == 'nt' else 'ex%02d' % num
    out = os.path.join(BUILD_DIR, exe)
    os.makedirs(BUILD_DIR, exist_ok=True)
    r = compile_ex(path, out)
    if r.returncode != 0:
        print(c(ANSI['r'], f'\n✗ 编译失败 {name}'))
        print(c(ANSI['dim'], '  编译器输出：'))
        for line in r.stderr.strip().split('\n')[-20:]:
            print('   ' + line)
        return False
    for stale in glob.glob(os.path.join(BUILD_DIR, 'wave*.vcd')):
        os.remove(stale)          # 防止上一轮残留文件让 ex24 误判通过
    try:
        p = subprocess.run([out], capture_output=True, timeout=15, cwd=BUILD_DIR,
                           env=dict(os.environ))
    except subprocess.TimeoutExpired:
        print(c(ANSI['r'], f'\n✗ 仿真超时 {name}（>15s）——多半是 wait 永远没等到或 sc_start 没停'))
        return False
    p.stdout, p.stderr = dec(p.stdout), dec(p.stderr)
    exp_file = os.path.join(TEST_DIR, 'ex%02d.expected' % num)
    expected = open(exp_file, encoding='utf-8').read() if os.path.exists(exp_file) else ''
    got, want = normalize(p.stdout), normalize(expected)
    if got == want and p.returncode == 0:
        return True
    print(c(ANSI['r'], f'\n✗ 输出不匹配 {name}'))
    if p.returncode != 0:
        err = (p.stderr or '').strip().split('\n')
        print(c(ANSI['y'], '  仿真报错（stderr 末尾）：'))
        for line in err[-6:]:
            print('   ' + line)
    import difflib
    diff = list(difflib.unified_diff(want.split('\n'), got.split('\n'),
                                     'expected', 'actual', lineterm='', n=1))
    shown = [d for d in diff if d.startswith(('+', '-', '@'))][:24]
    print()
    for d in shown:
        color = ANSI['g'] if d.startswith('---') or d.startswith('-') else ANSI['r']
        if d.startswith('+++') or d.startswith('+'):
            color = ANSI['r']
        print(c(color, '   ' + d))
    if len([d for d in diff if d.startswith(('+', '-'))]) > 24:
        print(c(ANSI['dim'], '   …（差异过长截断，verbose 模式看全部）'))
    return False


# ---------- commands ----------
def cmd_list():
    prog = load_progress()
    exs = all_exercises()
    done = 0
    print(f"\n learningsystemc  ·  {len(exs)} 关\n" + '─' * 46)
    for i, p in exs:
        m = re.match(r'ex\d+_(.*)\.cpp$', os.path.basename(p))
        title = (m.group(1) if m else os.path.basename(p)).replace('_', ' ')
        mark = c(ANSI['g'], '✓') if i in prog['done'] else c(ANSI['dim'], '·')
        done += 1 if i in prog['done'] else 0
        print(f"  {mark} ex{i:02d}  {title}")
    print('─' * 46)
    print(f"  进度 {done}/{len(exs)}\n")


def cmd_run(q=None):
    if q is None:
        num, path = next_exercise()
        if num is None:
            print(c(ANSI['g'], '\n🎉 全部 28 关通过！接下来建议：把 solutions/ 和你的写法 diff 一遍，然后去写个 TLM 模型吧。'))
            return
    else:
        num, path = find_ex(q)
        if num is None:
            print(c(ANSI['r'], f'找不到关卡 {q}，用 python lsx.py list 看看'))
            return
    ok = run_ex(num, path)
    if ok:
        prog = load_progress()
        if num not in prog['done']:
            prog['done'].append(num)
            save_progress(prog)
        print(c(ANSI['g'], f'\n✓ ex{num:02d} 过关！'))
        nxt, _ = next_exercise()
        if nxt is not None:
            print(c(ANSI['dim'], f'  下一关：python lsx.py next   (ex{nxt:02d})'))
        else:
            print(c(ANSI['g'], '  🎉 这是最后一关，通关了！'))
    else:
        print(c(ANSI['dim'], f'  卡住了？ python lsx.py hint ex{num:02d}'))


def cmd_hint(q):
    num, path = find_ex(q)
    if num is None:
        print(c(ANSI['r'], '找不到关卡')); return
    # hints live at the bottom of each exercise, after HINT marker
    src = open(path, encoding='utf-8').read()
    m = re.search(r'/\*\s*HINT\s*\*/\s*/\*([\s\S]*?)\*/', src)
    if not m:
        print('该关没有提示，检查 TODO 注释。'); return
    print(f"\n ex{num:02d} 提示：")
    for line in m.group(1).strip('\n').split('\n'):
        print('  ' + line)
    print()


def cmd_reset(q):
    num, path = find_ex(q)
    if num is None:
        print(c(ANSI['r'], '找不到关卡')); return
    sol = os.path.join(SOL_DIR, os.path.basename(path))
    # exercises ship from templates/ dir; solutions only for answer-checking
    tmpl = os.path.join(ROOT, 'templates', os.path.basename(path))
    src = tmpl if os.path.exists(tmpl) else sol
    if not os.path.exists(src):
        print('没有可恢复的模板'); return
    shutil.copy(src, path)
    print(f'已恢复 ex{num:02d} 初始代码' + ('（来自模板）' if os.path.exists(tmpl) else ''))


def cmd_verify():
    """Compile+run all solutions — maintainer check."""
    if not os.path.isdir(SOL_DIR):
        print('no solutions dir'); return
    fails = []
    for f in sorted(glob.glob(os.path.join(SOL_DIR, 'ex*.cpp'))):
        num = ex_id(f)
        ok = run_ex(num, f)
        print(f"  ex{num:02d} {'OK' if ok else 'FAIL'}")
        if not ok:
            fails.append(num)
    print('FAILS:', fails or 'none')


def main():
    # 中文 Windows 的 stdout 默认是 gbk，打 ✓/✗/🎉 会 UnicodeEncodeError；
    # 重定向到文件/管道时更明显，统一按 UTF-8 输出。
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding='utf-8', errors='replace')
        except Exception:
            pass
    _enable_ansi()
    a = sys.argv[1:]
    if not a or a[0] in ('list', 'ls'):
        cmd_list()
    elif a[0] == 'run':
        cmd_run(a[1] if len(a) > 1 else None)
    elif a[0] == 'next':
        cmd_run(None)
    elif a[0] == 'hint':
        cmd_hint(a[1] if len(a) > 1 else '')
    elif a[0] == 'reset':
        cmd_reset(a[1] if len(a) > 1 else '')
    elif a[0] == 'verify':
        cmd_verify()
    else:
        __doc__ and print(__doc__)


if __name__ == '__main__':
    main()
