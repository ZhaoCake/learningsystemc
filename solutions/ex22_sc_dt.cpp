#include <systemc.h>
int sc_main(int, char**) {
  sc_uint<8> a = 0xA5;
  std::cout << a << std::endl;
  sc_uint<8> hi = a >> 4;
  std::cout << hi << std::endl;
  sc_bv<4> v("1010");
  std::cout << v.to_string(sc_dt::SC_HEX) << std::endl;
  std::cout << v[0] << v[3] << std::endl;
  return 0;
}
