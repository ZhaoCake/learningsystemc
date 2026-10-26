// ex21 · sc_semaphore 计数信号量
// 目标：复现期望输出（容量 2，三个工人）
// 知识点：sc_semaphore 计数 / wait()/post() 阻塞语义
#include <systemc.h>

SC_MODULE(Worker) {
  sc_semaphore& s;
  const char* tag;
  int delay;
  Worker(sc_module_name nm, sc_semaphore& s_, const char* t_, int d_)
      : sc_module(nm), s(s_), tag(t_), delay(d_) { SC_HAS_PROCESS(Worker); SC_THREAD(run); }
  void run() {
    wait(delay, SC_NS);
    // TODO(1): 占一个名额（阻塞式），打印 "tag in @ t"
    wait(10, SC_NS);
    std::cout << tag << " out @ " << sc_time_stamp() << std::endl;
    // TODO(2): 释放名额
  }
};

int sc_main(int, char**) {
  sc_semaphore sem(2);                        // 同时允许 2 个
  Worker a("a", sem, "A", 0);
  Worker b("b", sem, "B", 1);
  Worker c("c", sem, "C", 2);
  sc_start(30, SC_NS);
  return 0;
}
/* HINT */ /*
semaphore(2)：头两个工人直接进（计数 2→0），第三个在 wait() 处排队。
A @0s 进、@10ns 出（post 后计数 +1，排队的 C 立刻被放行）；
B @1ns 进、@11ns 出；C in @10ns、out @20ns。
mutex 是"一次一个"的特例，semaphore 是"最多 N 个"的一般化——
资源池（DMA 通道、缓冲槽）建模就靠它。
和 ex20 对照着看，输出结构几乎一样，就多了 B/C 的并行进出场。
*/
