// ex08 · 初始化执行
// 目标：复现期望输出——注意 M 打印了几次、第一次在什么时刻
// 知识点：SC_METHOD 在 t=0 的"初始化执行"行为
#include <systemc.h>

SC_MODULE(Init) {
  sc_event go;
  SC_CTOR(Init) {
    SC_METHOD(m);
    // TODO(1): 让 m 对 go 敏感（注意：不要加 dont_initialize）
    SC_THREAD(t);
  }
  void m() { std::cout << "M runs @ " << sc_time_stamp() << std::endl; }
  void t() {
    wait(10, SC_NS);
    go.notify();
  }
};

int sc_main(int, char**) {
  Init x("x");
  sc_start();
  return 0;
}
/* HINT */ /*
不写 dont_initialize() 时，SC_METHOD 在仿真开始的初始化阶段就会执行一次——
哪怕敏感事件从没发生过。所以输出第一行是 @ 0 s，第二行才是 go 触发后的 @ 10 ns。
这个"空跑"经常坑新人：计数器莫名 +1、输出莫名多一行。
想关掉它就调用 dont_initialize()（前面几关一直默默用着它）。
*/
