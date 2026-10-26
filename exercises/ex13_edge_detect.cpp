// ex13 · 边沿检测
// 目标：信号四次翻转，rise/fall 各打印两次（对照期望）
// 知识点：posedge_event() / negedge_event() 作为敏感事件
#include <systemc.h>

SC_MODULE(Edge) {
  sc_signal<bool> s;
  SC_CTOR(Edge) {
    SC_METHOD(rise);
    sensitive << s.posedge_event();
    dont_initialize();
    SC_METHOD(fall);
    // TODO(1): 让 fall 对下降沿敏感
    dont_initialize();
    SC_THREAD(drv);
  }
  void rise() { std::cout << "rise @ " << sc_time_stamp() << std::endl; }
  void fall() { std::cout << "fall @ " << sc_time_stamp() << std::endl; }
  void drv() {
    wait(5, SC_NS);  s.write(true);
    wait(5, SC_NS);  s.write(false);
    wait(5, SC_NS);  s.write(true);
    wait(5, SC_NS);  s.write(false);
    wait(5, SC_NS);
  }
};

int sc_main(int, char**) {
  Edge e("e");
  sc_start(30, SC_NS);
  return 0;
}
/* HINT */ /*
bool 型 sc_signal 自带三个事件：default_event()（任何值变化）、
posedge_event()（0→1）、negedge_event()（1→0）。
模板里 rise 的写法照抄给 fall，把 posedge_event() 换成 negedge_event()。
翻转时刻：5/10/15/20 ns——true 是上升、false 是下降。
*/
