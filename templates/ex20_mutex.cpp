// ex20 · sc_mutex 互斥
// 目标：复现期望输出（B 必须等 A 用完才能进临界区）
// 知识点：sc_mutex lock/unlock 阻塞语义
#include <systemc.h>

SC_MODULE(Worker) {
  sc_mutex& m;
  const char* tag;
  int delay;
  Worker(sc_module_name nm, sc_mutex& m_, const char* t_, int d_)
      : sc_module(nm), m(m_), tag(t_), delay(d_) { SC_HAS_PROCESS(Worker); SC_THREAD(run); }
  void run() {
    wait(delay, SC_NS);
    // TODO(1): 进入临界区前加锁
    std::cout << tag << " in CS @ " << sc_time_stamp() << std::endl;
    wait(5, SC_NS);
    std::cout << tag << " out CS @ " << sc_time_stamp() << std::endl;
    // TODO(2): 离开临界区后解锁
  }
};

int sc_main(int, char**) {
  sc_mutex m;
  Worker a("a", m, "A", 0);
  Worker b("b", m, "B", 2);
  sc_start(20, SC_NS);
  return 0;
}
/* HINT */ /*
不加锁会怎样？B 在 2ns 直闯临界区，和 A 的输出交错——对照期望输出看差异。
sc_mutex.lock() 在别人持锁时会阻塞当前 SC_THREAD，unlock() 后按内核
调度唤醒。临界区里的 wait(5, SC_NS) 是模拟"持锁干活"——期间 B 全程卡在 lock。
unlock 别忘，否则 B 永远进不去（仿真 20ns 截断后少两行输出）。
*/
