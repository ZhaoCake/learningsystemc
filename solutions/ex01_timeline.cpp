#include <systemc.h>
SC_MODULE(Timeline) {
  SC_CTOR(Timeline) { SC_THREAD(run); }
  void run() {
    std::cout << "t = " << sc_time_stamp() << std::endl;
    wait(10, SC_NS);
    std::cout << "t = " << sc_time_stamp() << std::endl;
    wait(20, SC_NS);
    std::cout << "t = " << sc_time_stamp() << std::endl;
  }
};
int sc_main(int, char**) {
  Timeline tl("tl");
  sc_start();
  return 0;
}
