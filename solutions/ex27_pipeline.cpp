#include <systemc.h>
SC_MODULE(Prod) {
  sc_fifo<int>& out;
  Prod(sc_module_name nm, sc_fifo<int>& o) : sc_module(nm), out(o) {
    SC_HAS_PROCESS(Prod); SC_THREAD(run);
  }
  void run() {
    for (int i = 1; i <= 4; ++i) {
      out.write(i);
      wait(10, SC_NS);
    }
  }
};
SC_MODULE(Stage) {
  sc_fifo<int>& in;
  sc_fifo<int>& out;
  Stage(sc_module_name nm, sc_fifo<int>& i, sc_fifo<int>& o)
      : sc_module(nm), in(i), out(o) {
    SC_HAS_PROCESS(Stage); SC_THREAD(run);
  }
  void run() {
    for (;;) {
      int v = in.read();
      out.write(v * 2);
      std::cout << "stage: " << v << " -> " << v * 2 << " @ "
                << sc_time_stamp() << std::endl;
    }
  }
};
SC_MODULE(Sink) {
  sc_fifo<int>& in;
  Sink(sc_module_name nm, sc_fifo<int>& i) : sc_module(nm), in(i) {
    SC_HAS_PROCESS(Sink); SC_THREAD(run);
  }
  void run() {
    for (int i = 0; i < 4; ++i) {
      int v = in.read();
      std::cout << "sink: " << v << " @ " << sc_time_stamp() << std::endl;
    }
  }
};
int sc_main(int, char**) {
  sc_fifo<int> f1(1), f2(1);
  Prod p("p", f1);
  Stage s("s", f1, f2);
  Sink k("k", f2);
  sc_start();
  return 0;
}
