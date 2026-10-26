// ex10 · 值变化才触发
// 目标：复现期望输出——注意写三次、打印几次
// 知识点：sc_signal 的敏感条件是"值真的变了"
#include <systemc.h>

SC_MODULE(Watcher) {
  sc_signal<int> s;
  SC_CTOR(Watcher) {
    SC_METHOD(w);
    sensitive << s;
    dont_initialize();
    SC_THREAD(drv);
  }
  void w() { std::cout << "watch " << s.read() << " @ " << sc_time_stamp() << std::endl; }
  void drv() {
    s.write(7);
    wait(10, SC_NS);
    // TODO(1): 再写一次 7（同值），等 10ns
    // TODO(2): 最后写 9
  }
};

int sc_main(int, char**) {
  Watcher x("x");
  sc_start();
  return 0;
}
/* HINT */ /*
sensitive << s 触发条件不是"有人写了"，而是"值变了"（写后与旧值 == 比较）。
同值重写不产生事件——写 7 再写 7，watcher 只会在第一次醒。
这条规则是后面 handshake/valid-ready 波形分析的地基。
*/
