#include <systemc.h>
SC_MODULE(Init) {
  sc_event go;
  SC_CTOR(Init) {
    SC_METHOD(m);
    sensitive << go;
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
