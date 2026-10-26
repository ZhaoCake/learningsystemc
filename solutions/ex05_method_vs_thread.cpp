#include <systemc.h>
SC_MODULE(Two) {
  sc_signal<int> sig;
  SC_CTOR(Two) {
    SC_METHOD(m);
    sensitive << sig;
    dont_initialize();
    SC_THREAD(t);
  }
  void m() { std::cout << "M @ " << sc_time_stamp() << std::endl; }
  void t() {
    std::cout << "T begin" << std::endl;
    sig.write(1);
    wait(10, SC_NS);
    sig.write(2);
    wait(10, SC_NS);
    std::cout << "T end" << std::endl;
  }
};
int sc_main(int, char**) {
  Two x("x");
  sc_start();
  return 0;
}
