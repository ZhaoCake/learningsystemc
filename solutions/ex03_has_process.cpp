#include <systemc.h>
class Timer : public sc_module {
 public:
  SC_HAS_PROCESS(Timer);
  Timer(sc_module_name nm) : sc_module(nm) {
    SC_THREAD(tick);
  }
  void tick() {
    for (int i = 1; i <= 3; ++i) {
      wait(5, SC_NS);
      std::cout << "tick " << i << " @ " << sc_time_stamp() << std::endl;
    }
  }
};
int sc_main(int, char**) {
  Timer tmr("tmr");
  sc_start();
  return 0;
}
