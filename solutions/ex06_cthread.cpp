#include <systemc.h>
SC_MODULE(Counter) {
  sc_in<bool> clk;
  SC_CTOR(Counter) {
    SC_CTHREAD(tick, clk.pos());
  }
  void tick() {
    int cnt = 0;
    for (;;) {
      wait();
      cnt++;
      std::cout << "cnt=" << cnt << " @ " << sc_time_stamp() << std::endl;
    }
  }
};
int sc_main(int, char**) {
  sc_clock clk("clk", 10, SC_NS);
  Counter c("c");
  c.clk(clk);
  sc_start(35, SC_NS);
  return 0;
}
