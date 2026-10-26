#include <systemc.h>
int sc_main(int, char**) {
  sc_time t1(10, SC_NS);
  sc_time t2(1.5, SC_US);
  sc_time t3 = t1 + t2;
  std::cout << t1 << " " << t2 << " " << t3 << std::endl;
  sc_start(2.5, SC_MS);
  std::cout << sc_time_stamp() << std::endl;
  return 0;
}
