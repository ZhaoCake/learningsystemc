// ex02 · 层次化命名
// 目标：输出 top.dut_a 和 top.dut_b 两行
// 知识点：sc_module 的 name() / 层次化实例化
#include <systemc.h>

SC_MODULE(Child) {
  SC_CTOR(Child) { SC_METHOD(show); }
  void show() { std::cout << name() << std::endl; }
};

SC_MODULE(Parent) {
  SC_CTOR(Parent) {
    // TODO(1): 用名字 "dut_a" 实例化一个 Child
    // TODO(2): 再用名字 "dut_b" 实例化一个 Child
    // 注意：完整名字 = 父名.子名，会自动生成，不要手工拼接字符串
  }
};

int sc_main(int, char**) {
  Parent top("top");
  sc_start(1, SC_NS);
  return 0;
}
/* HINT */ /*
SC_CTOR(Child) 展开后构造函数接受一个 sc_module_name 参数，
实例化时把名字传进去即可：Child a("dut_a")。
关键陷阱：子模块必须是 Parent 的【成员对象】（成员初始化列表里给名字），
不能是构造函数体里的局部变量——局部变量在构造结束时就析构了，
仿真开始前内核里的子模块已经消失。
成员写法：
  SC_MODULE(Parent) {
    Child a, b;
    SC_CTOR(Parent) : a("dut_a"), b("dut_b") {}
  };
*/
