// ex03 · SC_HAS_PROCESS
// 目标：输出三行 tick
// 知识点：不用 SC_CTOR 时，如何让 SC_THREAD/SC_METHOD 可用
#include <systemc.h>

class Timer : public sc_module {
 public:
  SC_HAS_PROCESS(Timer);
  Timer(sc_module_name nm) : sc_module(nm) {
    // TODO(1): 注册 SC_THREAD(tick)
  }
  void tick() {
    for (int i = 1; i <= 3; ++i) {
      wait(5, SC_NS);
      std::cout << "tick " << i << " @ " << sc_time_stamp() << std::endl;
    }
  }
};

int sc_main(int, char**) {
  Timer tmr("tmr");
  sc_start();
  return 0;
}
/* HINT */ /*
SC_CTOR 之所以"顺带"能让 SC_THREAD 编译通过，是因为它内部声明了
SC_HAS_PROCESS。当构造函数是手写的时候，这个声明就没人替你做了——
好在模板里已经写好 SC_HAS_PROCESS(Timer)，你只需要注册进程本体。
和 ex00 的 SC_METHOD 不同：线程里的 wait 会挂起自己，所以 for 循环能跑三轮。
*/
