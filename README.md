# learningsystemc 🔬

像 [rustlings](https://github.com/rust-lang/rustlings) 一样，一关一关过 SystemC 语法关。

每个练习是一个**跑不通的小程序**：补全 `TODO` 让仿真输出与期望一致，即过关。
本仓库不教概念——概念请配合《SystemC 入门教程（中文版）》对应章节（见 [STUDY.md](STUDY.md) 对照表）。

> 本人让AI写了之后正在试用中。如果不错，后面会更新README。

## 环境准备

不用 Docker，也不用自己编 SystemC：直接吃本仓库 `third_party/` 里已经编好的那一套。

| 项 | 值 |
|---|---|
| 编译器 | MSVC v143（VS 2022 BuildTools），`lsx.py` 用 `vswhere` 自动定位，不用先开 Developer Prompt |
| SystemC | `../third_party/systemc-install`（2.3.4，C++17，静态库 `lib/systemc.lib`） |
| Python | 3.8+，只用标准库 |

runner 实际拼出来的命令（Windows）：

```
cl /nologo /std:c++17 /EHsc /MD /vmg /utf-8 /Od /I <install>/include <ex>.cpp
   /Fe:<build>/exNN.exe /link /LIBPATH:<install>/lib systemc.lib /SUBSYSTEM:CONSOLE
```

几个不能省的坑：

- **`/vmg` 必须有**。`SC_METHOD` / `SC_THREAD` 宏要把成员函数指针转成统一表示，
  不加就是 `warning C4407` + 一进 `sc_start` 直接 `0xC0000005`（无输出、无栈信息，很难查）。
- **`/SUBSYSTEM:CONSOLE` 要显式给**。SystemC 的 `main()` 在 `systemc.lib` 里（`sc_main.cpp`），
  不显式指定子系统时 LINK 找不到入口点，报 `LNK1561: 必须定义入口点`。
- **`/MD` 与库必须一致**。`systemc.lib` 是 Release（`/MD`）编的，练习也得 `/MD`，
  否则 `LNK2038: RuntimeLibrary 不匹配`。所以这里也不支持切到 `third_party/systemc-install-debug`。
- **别换 g++**。本机那个 MinGW g++ 8.1 是 `win32` 线程模型（没有 `std::thread`），
  而且 MSVC 编出来的 `systemc.lib` 是 MSVC ABI，g++ 链接不上。

需要换位置时用环境变量：

```powershell
$env:SYSTEMC_HOME = "d:\WorkSpace\systemc_playground\third_party\systemc-install"  # 含 include/systemc.h 与 lib/systemc.lib 的前缀
$env:LSC_CXX      = "g++"   # 强制走 GCC 路径（仅在 SystemC 本身是 GCC 编的时候才有意义）
```

## 怎么玩

```powershell
python lsx.py list          # 全部关卡与进度（✓ 已过 / · 未开始）
python lsx.py next          # 直接跑下一关
python lsx.py run 7         # 跑指定关卡（也支持 ex07、完整文件名）
python lsx.py hint 7        # 看提示（卡住再看！）
python lsx.py reset 7       # 想重做某关，恢复初始代码
python lsx.py verify        # 维护用：把 solutions/ 全部参考解答编译+跑一遍
```

编辑器里改 `exercises/exNN_*.cpp` → 再跑 `python lsx.py next`。
输出与 `tests/` 里的期望一致就过关，进度记在 `.lsx_progress.json`；可执行文件和 `.obj` 都丢在 `build/`。
（Windows 上命令是 `python`，不是 `python3`。）

**规则**：只改 `TODO` 标记的地方；不要动期望输出。卡壳超过 15 分钟就看提示，
看了解提示还不动手等于没练。

## 编辑器补全 / 跳转（clangd）

本目录的 `.clangd` 给编辑器用，指向 `../third_party/systemc-install/include`、`-std=c++17`：

- 只把 `learningsystemc/` 作为工作区打开时，用本目录这份 `.clangd`；
- 打开整个 `systemc_playground/` 时，根目录也有一份同样 flag 的 `.clangd`，内容一致。
- 里面的 include 是**绝对路径**，换盘符或挪仓库位置要跟着改。
- 装了 clangd 18 + VS 2022 17.14 的组合会每个文件报 `error STL1000: Unexpected compiler version`，
  所以配置里加了 `-D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH`；升级到 clangd 19+ 后可删。

## 目录结构

```
exercises/   28 个关卡（你的战场，只改这里）
tests/       期望输出（判题依据，别改）
solutions/   参考解答（实在卡死再看）
templates/   reset 用的初始模板
build/       编译产物（gitignore，随便删）
lsx.py       判题 runner（MSVC + third_party/systemc-install）
.clangd      编辑器语言服务配置
STUDY.md     关卡 ↔ 教程章节对照 + 建议节奏
```

## 关卡地图

| 章 | 关卡数 | 主题 |
|----|--------|------|
| 0  | 2  | 起步：模块、仿真启动 |
| 1  | 3  | 模块与构造：SC_CTOR / SC_HAS_PROCESS / 层次化 |
| 2  | 4  | 进程：SC_METHOD / SC_THREAD / SC_CTHREAD / 敏感与初始化 |
| 3  | 5  | 信号与端口：delta 语义 / 绑定 / 多驱动 / 时钟 |
| 4  | 4  | 事件与时间：notify / wait 超时 / 与或事件表 |
| 5  | 4  | 通道：sc_fifo / sc_mutex / sc_semaphore |
| 6  | 3  | 数据类型与诊断：sc_int/sc_bv / sc_report / VCD 波形 |
| 7  | 3  | 综合模式：busy 重入 / 中断 / 生产者消费者流水线 |
