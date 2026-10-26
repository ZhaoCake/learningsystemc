#include <systemc.h>
SC_MODULE(Prod) {
  sc_fifo<int>& f;
  Prod(sc_module_name nm, sc_fifo<int>& f_) : sc_module(nm), f(f_) { SC_HAS_PROCESS(Prod); SC_THREAD(run); }
  void run() {
    wait(15, SC_NS);
    f.write(5);
    wait(20, SC_NS);
  }
};
SC_MODULE(Cons) {
  sc_fifo<int>& f;
  Cons(sc_module_name nm, sc_fifo<int>& f_) : sc_module(nm), f(f_) { SC_HAS_PROCESS(Cons); SC_THREAD(run); }
  void run() {
    while (true) {
      int v;
      if (f.nb_read(v)) {
        std::cout << "got " << v << " @ " << sc_time_stamp() << std::endl;
        break;
      }
      std::cout << "empty @ " << sc_time_stamp() << std::endl;
      wait(10, SC_NS);
    }
  }
};
int sc_main(int, char**) {
  sc_fifo<int> fifo(1);
  Prod p("p", fifo);
  Cons c("c", fifo);
  sc_start(50, SC_NS);
  return 0;
}
