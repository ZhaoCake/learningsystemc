// ex05 · SC_METHOD vs SC_THREAD
// 目标：复现期望输出（注意 M 出现几次、各在什么时刻）
// 知识点：method 每次触发重新执行 / thread 一生执行一次、靠 wait 推进
#include <systemc.h>

SC_MODULE(Two) {
  sc_signal<int> sig;
  SC_CTOR(Two) {
    SC_METHOD(m);
    // TODO(1): 给 m 加上对 sig 的静态敏感
    dont_initialize();
    SC_THREAD(t);
  }
  void m() { std::cout << "M @ " << sc_time_stamp() << std::endl; }
  void t() {
    std::cout << "T begin" << std::endl;
    sig.write(1);
    wait(10, SC_NS);
    // TODO(2): 再写一次 sig（值写成 2），再等 10 ns —— 让 M 多触发一次
    std::cout << "T end" << std::endl;
  }
};

int sc_main(int, char**) {
  Two x("x");
  sc_start();
  return 0;
}
/* HINT */ /*
SC_METHOD 没有自己的执行流：每次敏感事件发生，它从头到尾跑一遍（不能 wait）。
SC_THREAD 从头跑到尾（可以 wait），不重复进入——除非 dont_initialize 被
关掉且在敏感表上，才会在 t=0 额外跑一次。
sig.write(v) 触发事件是在【下一个 delta 周期】，但时间戳不变。
*/
