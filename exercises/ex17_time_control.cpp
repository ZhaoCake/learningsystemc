// ex17 · 仿真时间控制
// 目标：复现期望输出
// 知识点：sc_start(duration) 定时停 / sc_time_stamp 在 sc_main 里的值
#include <systemc.h>

SC_MODULE(Ticker) {
  SC_CTOR(Ticker) { SC_THREAD(run); }
  void run() {
    for (int i = 0; i < 10; ++i) {
      wait(5, SC_NS);
      std::cout << "tick " << i << " @ " << sc_time_stamp() << std::endl;
    }
  }
};

int sc_main(int, char**) {
  Ticker t("t");
  // TODO(1): 让仿真跑 17ns 就停（时间一到自动返回，ticker 被掐断）
  sc_start(5, SC_NS);
  std::cout << "main resumes @ " << sc_time_stamp() << std::endl;
  return 0;
}
/* HINT */ /*
sc_start(17, SC_NS)：仿真推进到 17ns，不管进程还有没有没跑完的事。
ticker 本想打 10 个 tick，但时间到就被掐——只打印 @5/10/15 三次。
sc_start 返回后，sc_main 的 sc_time_stamp() 就是停止时刻。
无参 sc_start() 会跑到没有任何事件才停——本例 ticker 每 5ns 一跳，
永不消停，sc_main 永远回不来（想想就可怕）。
*/
