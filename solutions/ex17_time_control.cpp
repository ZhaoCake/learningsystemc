#include <systemc.h>
SC_MODULE(Ticker) {
  SC_CTOR(Ticker) { SC_THREAD(run); }
  void run() {
    for (int i = 0; i < 10; ++i) {
      wait(5, SC_NS);
      std::cout << "tick " << i << " @ " << sc_time_stamp() << std::endl;
    }
  }
};
int sc_main(int, char**) {
  Ticker t("t");
  sc_start(17, SC_NS);
  std::cout << "main resumes @ " << sc_time_stamp() << std::endl;
  return 0;
}
