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
    ev.notify();
    wait(10, SC_NS);
    ev.notify(5, SC_NS);
    wait(20, SC_NS);
  }
};
int sc_main(int, char**) {
  Notify n("n");
  sc_start();
  return 0;
}
