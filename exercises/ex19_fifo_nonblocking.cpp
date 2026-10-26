// ex19 · 非阻塞接口
// 目标：复现期望输出
// 知识点：nb_read / 轮询 / num_available
#include <systemc.h>

SC_MODULE(Prod) {
  sc_fifo<int>& f;
  Prod(sc_module_name nm, sc_fifo<int>& f_) : sc_module(nm), f(f_) { SC_HAS_PROCESS(Prod); SC_THREAD(run); }
  void run() {
    wait(15, SC_NS);           // 故意晚来
    f.write(5);
    wait(20, SC_NS);
  }
};

SC_MODULE(Cons) {
  sc_fifo<int>& f;
  Cons(sc_module_name nm, sc_fifo<int>& f_) : sc_module(nm), f(f_) { SC_HAS_PROCESS(Cons); SC_THREAD(run); }
  void run() {
    while (true) {
      int v;
      // TODO(1): 用非阻塞方式尝试取：取到就打印 "got v"，没取到打印 "empty @ t" 并等 10ns
      v = f.read();
      std::cout << "got " << v << " @ " << sc_time_stamp() << std::endl;
      wait(10, SC_NS);
      if (v == 5) break;
    }
  }
};

int sc_main(int, char**) {
  sc_fifo<int> fifo(1);
  Prod p("p", fifo);
  Cons c("c", fifo);
  sc_start(50, SC_NS);
  return 0;
}
/* HINT */ /*
nb_read(v) 返回 bool：true 表示取到了，false 表示 fifo 现在是空的——
绝不阻塞。轮询 + nb_read 是"消费者比生产者快"场景的标准写法。
期望时序：0s 空、10ns 空、15ns prod 写入、20ns 取到 5。
nb_ 前缀的其他变体（nb_write）同理。num_available() 还能先探队尾深度。
*/
