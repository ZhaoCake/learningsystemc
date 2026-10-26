#include <systemc.h>
SC_MODULE(Timeout) {
  sc_event ev;
  SC_CTOR(Timeout) { SC_THREAD(t); }
  void t() {
    ev.notify(20, SC_NS);
    wait(10, SC_NS, ev);
    std::cout << "first wait done @ " << sc_time_stamp() << std::endl;
    wait(15, SC_NS, ev);
    std::cout << "second wait done @ " << sc_time_stamp() << std::endl;
  }
};
int sc_main(int, char**) {
  Timeout x("x");
  sc_start(40, SC_NS);
  return 0;
}
