// ex16 · 事件组合：or_list 与 and_list
// 目标：复现期望输出
// 知识点：任一事件 / 全部事件 的等待语法
#include <systemc.h>

SC_MODULE(Barriers) {
  sc_event e1, e2;
  SC_CTOR(Barriers) {
    SC_THREAD(any_waiter);
    SC_THREAD(all_waiter);
    SC_THREAD(driver);
  }
  void any_waiter() {
    sc_event_or_list any = e1 | e2;
    wait(any);
    std::cout << "any fired @ " << sc_time_stamp() << std::endl;
  }
  void all_waiter() {
    sc_event_and_list all = e1 & e2;
    wait(all);
    std::cout << "all fired @ " << sc_time_stamp() << std::endl;
  }
  void driver() {
    e2.notify(5, SC_NS);
    // TODO(1): 10ns 时触发 e1
  }
};

int sc_main(int, char**) {
  Barriers b("b");
  sc_start(30, SC_NS);
  return 0;
}
/* HINT */ /*
sc_event_or_list：任一成员触发即唤醒（5ns e2 一到就走）。
sc_event_and_list：每个成员都至少触发一次才唤醒（10ns e1 补齐最后一块）。
TODO(1) 不补的话 all_waiter 永远等不齐——幸好 sc_start(30, SC_NS) 有时限，
仿真到点强制结束，and 那行就打印不出来。
事件组合可以做"join 屏障"和"多源中断聚合"，RTL 验证里很常用。
*/
