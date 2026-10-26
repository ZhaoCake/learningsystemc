// ex18 · sc_fifo 阻塞语义
// 目标：复现期望输出（注意 get 的顺序与时刻）
// 知识点：sc_fifo 满则写阻塞 / 空则读阻塞 / 容量 2 的行为
#include <systemc.h>

SC_MODULE(Prod) {
  sc_fifo<int>& f;
  SC_CTOR(Prod) : f(*(new sc_fifo<int>(0))) {}   // 占位，实际用下面的带参构造
  Prod(sc_module_name nm, sc_fifo<int>& f_) : sc_module(nm), f(f_) { SC_HAS_PROCESS(Prod); SC_THREAD(run); }
  void run() {
    f.write(1);
    wait(10, SC_NS);
    f.write(2);
    wait(10, SC_NS);
    // TODO(1): 依次写入 3 和 4（都不带等待间隔，体会"满则阻塞"）
  }
};

SC_MODULE(Cons) {
  sc_fifo<int>& f;
  Cons(sc_module_name nm, sc_fifo<int>& f_) : sc_module(nm), f(f_) { SC_HAS_PROCESS(Cons); SC_THREAD(run); }
  void run() {
    for (int i = 0; i < 4; ++i) {
      int v = f.read();
      std::cout << "got " << v << " @ " << sc_time_stamp() << std::endl;
      // TODO(2): 每取一个歇 5ns
    }
  }
};

int sc_main(int, char**) {
  sc_fifo<int> fifo(2);            // 容量 2
  Prod p("p", fifo);
  Cons c("c", fifo);
  sc_start(40, SC_NS);   // 时限保护：卡死时仿真到点自动结束
  return 0;
}
/* HINT */ /*
sc_fifo<int> fifo(2)：容量 2。@0s 时 prod 连写两个都成功，第三个 write 阻塞，
直到 cons get 腾出位置。阻塞式 read/write 是 sc_fifo 的灵魂——
天然就是生产者-消费者的同步原语，不需要手写握手。
时序推演（写完自己先推一遍再看输出）：
@0s 写 1，cons 取 1；prod 的 wait(10) 让后续写入每 10ns 一个——
cons 在 5ns/15ns 想读时 fifo 是空的，read() 阻塞到下一个写入：
1@0s, 2@10ns, 3@20ns, 4@25ns（3 和 4 是 prod 连写的，所以只隔 5ns）。
注意模块持有通道要用引用成员 sc_fifo<int>&，通道本体放在 sc_main。
*/
