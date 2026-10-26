// ex07 · 静态敏感 vs 动态触发
// 目标：M 打印两次（对照期望）
// 知识点：sensitive << event 是静态绑定；event.notify() 是动态触发
#include <systemc.h>

SC_MODULE(Switcher) {
  sc_event e1, e2;
  SC_CTOR(Switcher) {
    SC_METHOD(m);
    sensitive << e1;
    dont_initialize();
    SC_THREAD(driver);
  }
  void m() { std::cout << "M sees e1 @ " << sc_time_stamp() << std::endl; }
  void driver() {
    e1.notify(5, SC_NS);      // 5ns 后触发 e1 → M 第一次打印
    wait(10, SC_NS);
    e2.notify(0, SC_NS);      // e2 被触发，但 M 不关心它
    wait(5, SC_NS);
    // TODO(1): 再触发一次 e1（立即通知即可），让 M 第二次打印
  }
};

int sc_main(int, char**) {
  Switcher s("s");
  sc_start();
  return 0;
}
/* HINT */ /*
notify() 立即触发（当前 delta 内排队），notify(delay, unit) 定时触发。
e2 没人敏感，触发等于石沉大海——事件不是广播，只有挂在敏感表/wait 上的人收得到。
第二次 e1.notify() 放在 driver 的最后，时间戳是 15 ns。
*/
