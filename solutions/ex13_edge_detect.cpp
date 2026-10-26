#include <systemc.h>
SC_MODULE(Edge) {
  sc_signal<bool> s;
  SC_CTOR(Edge) {
    SC_METHOD(rise);
    sensitive << s.posedge_event();
    dont_initialize();
    SC_METHOD(fall);
    sensitive << s.negedge_event();
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
