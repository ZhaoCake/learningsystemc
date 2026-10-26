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
    s.write(7);
    wait(10, SC_NS);
    s.write(9);
  }
};
int sc_main(int, char**) {
  Watcher x("x");
  sc_start();
  return 0;
}
