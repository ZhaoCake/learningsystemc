// ex06 · SC_CTHREAD 时钟进程
// 目标：输出 6 行 cnt（对照 tests/ex06_cthread.expected）
// 知识点：SC_CTHREAD / sc_clock / 永续进程写法
#include <systemc.h>

SC_MODULE(Counter) {
  sc_in<bool> clk;
  SC_CTOR(Counter) {
    // TODO(1): 注册 SC_CTHREAD(tick, clk.pos()) —— 敏感于时钟上升沿
  }
  void tick() {
    int cnt = 0;
    for (;;) {
      wait();           // 等下一个上升沿（敏感表已经声明了 pos）
      cnt++;
      std::cout << "cnt=" << cnt << " @ " << sc_time_stamp() << std::endl;
    }
  }
};

int sc_main(int, char**) {
  sc_clock clk("clk", 10, SC_NS);   // 周期 10ns，默认占空比 0.5：首个上升沿在 5ns
  Counter c("c");
  c.clk(clk);
  sc_start(35, SC_NS);
  return 0;
}
/* HINT */ /*
SC_CTHREAD(f, clk.pos()) = SC_THREAD + 静态敏感于 f(clk.pos())。
SC_CTHREAD 的函数体里，wait() 不带参数就是"等下一个声明的沿"。
sc_clock 相位有讲究：默认构造下，本环境实测 SC_CTHREAD 首次被
上升沿唤醒是在 10ns（= 一个完整周期），不是直觉上的 5ns。
做这关前先想清楚"wait() 等的是沿事件，不是电平"，再对照期望输出校准认知。
*/
