// ex09 · delta 语义
// 目标：复现期望输出——第一行是 0 还是 42？
// 知识点：sc_signal 的"现在读到旧值、下个 delta 见新值"
#include <systemc.h>

SC_MODULE(Delta) {
  sc_signal<int> s;
  SC_CTOR(Delta) { SC_THREAD(run); }
  void run() {
    s.write(42);
    std::cout << "read now: " << s.read() << std::endl;
    // TODO(1): 只等一个 delta 周期（不消耗仿真时间），再读一次
    std::cout << "read after delta: " << s.read() << std::endl;
  }
};

int sc_main(int, char**) {
  Delta d("d");
  sc_start();
  return 0;
}
/* HINT */ /*
sc_signal.write() 只是"预订"新值：当前 delta 内 read() 拿到的仍是旧值，
更新发生在下一个 delta 周期（evaluate-update 两阶段）。
wait(SC_ZERO_TIME) 恰好推进一个 delta、不消耗仿真时间——这是观察
delta 语义的标准姿势。wait(0, SC_NS) 等价。
*/
