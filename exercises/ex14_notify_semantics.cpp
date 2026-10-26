// ex14 · notify 的两种姿势
// 目标：复现期望输出（事件在什么时刻被触发）
// 知识点：notify() 立即触发 vs notify(delay) 定时触发
#include <systemc.h>

SC_MODULE(Notify) {
  sc_event ev;
  SC_CTOR(Notify) {
    SC_METHOD(m);
    sensitive << ev;
    dont_initialize();
    SC_THREAD(t);
  }
  void m() { std::cout << "fired @ " << sc_time_stamp() << std::endl; }
  void t() {
    // TODO(1): 立即触发一次 ev
    wait(10, SC_NS);
    // TODO(2): 定时 5ns 后再触发一次 ev
    wait(20, SC_NS);
  }
};

int sc_main(int, char**) {
  Notify n("n");
  sc_start();
  return 0;
}
/* HINT */ /*
notify() 无参：事件在本 delta 内被触发（时间戳不变）。
notify(5, SC_NS)：事件在"未来 5ns"触发——相当于给自己定了闹钟。
注意 notify 是"覆盖式"的：同一事件再次 notify 会覆盖尚未发生的定时通知。
*/
