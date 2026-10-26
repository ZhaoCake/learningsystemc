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
    e1.notify(5, SC_NS);
    wait(10, SC_NS);
    e2.notify(0, SC_NS);
    wait(5, SC_NS);
    e1.notify();
  }
};
int sc_main(int, char**) {
  Switcher s("s");
  sc_start();
  return 0;
}
