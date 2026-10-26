#include <systemc.h>
SC_MODULE(Hello) {
  SC_CTOR(Hello) { SC_METHOD(hello); }
  void hello() { std::cout << "Hello SystemC!" << std::endl; }
};
int sc_main(int, char**) {
  Hello h("hello");
  sc_start(1, SC_NS);
  return 0;
}
