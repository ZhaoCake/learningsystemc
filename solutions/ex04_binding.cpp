#include <systemc.h>
SC_MODULE(Producer) {
  sc_out<int> out;
  SC_CTOR(Producer) { SC_THREAD(run); }
  void run() {
    int v = 1;
    for (int i = 0; i < 3; ++i) {
      out.write(v);
      wait(10, SC_NS);
      v *= 2;
    }
  }
};
SC_MODULE(Consumer) {
  sc_in<int> in;
  SC_CTOR(Consumer) {
    SC_METHOD(show);
    sensitive << in;
    dont_initialize();
  }
  void show() { std::cout << "got " << in.read() << " @ " << sc_time_stamp() << std::endl; }
};
SC_MODULE(Top) {
  sc_signal<int> link;
  Producer prod;
  Consumer cons;
  SC_CTOR(Top) : prod("prod"), cons("cons") {
    prod.out(link);
    cons.in(link);
  }
};
int sc_main(int, char**) {
  Top top("top");
  sc_start();
  return 0;
}
