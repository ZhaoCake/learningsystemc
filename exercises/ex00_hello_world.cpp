// ex00 · Hello SystemC
// 目标：让程序输出一行 "Hello SystemC!"（与 tests/ex00_hello_world.expected 完全一致）
// 知识点：SC_MODULE / SC_METHOD 注册 / sc_start
// 规则：只改 TODO 处
#include <systemc.h>

SC_MODULE(Hello) {
  SC_CTOR(Hello) {
    // TODO(1): 把 hello 注册为 SC_METHOD 进程，内核才会调度它
  }
  void hello() { std::cout << "Hello SystemC!" << std::endl; }
};

int sc_main(int, char**) {
  Hello h("hello");
  // TODO(2): 启动仿真（不启动，内核什么都不做）
  return 0;
}
/* HINT */ /*
SystemC 的进程不是普通函数调用——必须在构造函数里用 SC_METHOD(函数名) 登记，
sc_main 里用 sc_start() 交给内核，内核才会按语义执行已注册的进程。
sc_start(1, SC_NS) 跑 1 纳秒就够这一关。
*/
