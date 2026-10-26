// ex24 · VCD 波形追踪
// 目标：让程序生成 wave.vcd，并打印文件第一行来验证
// 知识点：sc_create_vcd_trace_file / sc_trace / sc_close_vcd_trace_file
#include <systemc.h>
#include <fstream>

SC_MODULE(Counter) {
  sc_signal<int> cnt;
  SC_CTOR(Counter) { SC_THREAD(run); }
  void run() {
    for (int i = 0; i < 3; ++i) {
      cnt.write(i * 10);
      wait(10, SC_NS);
    }
  }
};

int sc_main(int, char**) {
  Counter c("c");
  // TODO(1): 创建 VCD 追踪文件（名字用 "wave"）
  // TODO(2): 把 c 的 cnt 信号加进追踪（信号名 "cnt"）
  sc_start(30, SC_NS);
  // TODO(3): 关闭追踪文件（不关的话数据可能没刷进磁盘）
  std::ifstream f("wave.vcd");
  std::string line;
  bool found = false;
  while (std::getline(f, line))
    if (line.rfind("$date", 0) == 0) { found = true; break; }
  std::cout << (found ? "vcd header OK" : "vcd header missing") << std::endl;
  return 0;
}
/* HINT */ /*
三步曲：create → trace(文件, 信号, 波形里显示的名字) → close。
波形文件对验证至关重要：Questa/GTKWave 打开 wave.vcd 就能看信号翻转。
close 忘了调用，缓冲区没刷新，文件可能是空的——读首行就是空输出。
文件会生成在运行目录（lsx 环境下是 build/），不会污染你的练习目录。
*/
