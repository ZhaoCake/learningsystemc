#include <systemc.h>
SC_MODULE(Child) {
  SC_CTOR(Child) { SC_METHOD(show); }
  void show() { std::cout << name() << std::endl; }
};
SC_MODULE(Parent) {
  Child a, b;
  SC_CTOR(Parent) : a("dut_a"), b("dut_b") {
    // 子模块必须是成员对象：局部变量会在构造函数结束时析构
  }
};
int sc_main(int, char**) {
  Parent top("top");
  sc_start(1, SC_NS);
  return 0;
}
