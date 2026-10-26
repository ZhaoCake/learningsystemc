#include <systemc.h>
SC_MODULE(Prod) {
  sc_fifo<int>& f;
  Prod(sc_module_name nm, sc_fifo<int>& f_) : sc_module(nm), f(f_) { SC_HAS_PROCESS(Prod); SC_THREAD(run); }
  void run() {
    f.write(1);
    wait(10, SC_NS);
    f.write(2);
    wait(10, SC_NS);
    f.write(3);
    f.write(4);
  }
};
SC_MODULE(Cons) {
  sc_fifo<int>& f;
  Cons(sc_module_name nm, sc_fifo<int>& f_) : sc_module(nm), f(f_) { SC_HAS_PROCESS(Cons); SC_THREAD(run); }
  void run() {
    for (int i = 0; i < 4; ++i) {
      int v = f.read();
      std::cout << "got " << v << " @ " << sc_time_stamp() << std::endl;
      wait(5, SC_NS);
    }
  }
};
int sc_main(int, char**) {
  sc_fifo<int> fifo(2);
  Prod p("p", fifo);
  Cons c("c", fifo);
  sc_start();
  return 0;
}
