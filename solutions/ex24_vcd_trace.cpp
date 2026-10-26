#include <systemc.h>
#include <fstream>
SC_MODULE(Counter) {
  sc_signal<int> cnt;
  SC_CTOR(Counter) { SC_THREAD(run); }
  void run() {
    for (int i = 0; i < 3; ++i) {
      cnt.write(i * 10);
      wait(10, SC_NS);
    }
  }
};
int sc_main(int, char**) {
  Counter c("c");
  sc_trace_file* tf = sc_create_vcd_trace_file("wave");
  sc_trace(tf, c.cnt, "cnt");
  sc_start(30, SC_NS);
  sc_close_vcd_trace_file(tf);
  std::ifstream f("wave.vcd");
  std::string line;
  bool found = false;
  while (std::getline(f, line))
    if (line.rfind("$date", 0) == 0) { found = true; break; }
  std::cout << (found ? "vcd header OK" : "vcd header missing") << std::endl;
  return 0;
}
