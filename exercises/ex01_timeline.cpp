// ex01 · 时间线
// 目标：让第三行输出 t = 30 ns（前两行已正确）
// 知识点：SC_THREAD / wait(n, SC_NS) / sc_time_stamp
#include <systemc.h>

SC_MODULE(Timeline) {
  SC_CTOR(Timeline) { SC_THREAD(run); }
  void run() {
    std::cout << "t = " << sc_time_stamp() << std::endl;
    wait(10, SC_NS);
    std::cout << "t = " << sc_time_stamp() << std::endl;
    // TODO(1): 在这里等待一段合适的时间，让下一个时间戳是 30 ns
    std::cout << "t = " << sc_time_stamp() << std::endl;
  }
};

int sc_main(int, char**) {
  Timeline tl("tl");
  sc_start();
  return 0;
}
/* HINT */ /*
SC_THREAD 在 wait() 处挂起，时间被推进指定量后从 wait 的下一行恢复。
第二行已经是 10 ns，要让第三行是 30 ns，再等多少？
*/
