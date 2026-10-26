// ex27 · 终极关：三级流水线
// 目标：producer → fifo → stage(×2) → fifo → sink，复现期望输出
// 知识点：fifo 背压天然形成流水线节拍
#include <systemc.h>

SC_MODULE(Prod) {
  sc_fifo<int>& out;
  Prod(sc_module_name nm, sc_fifo<int>& o) : sc_module(nm), out(o) {
    SC_HAS_PROCESS(Prod); SC_THREAD(run);
  }
  void run() {
    for (int i = 1; i <= 4; ++i) {
      out.write(i);
      wait(10, SC_NS);
    }
  }
};

SC_MODULE(Stage) {
  sc_fifo<int>& in;
  sc_fifo<int>& out;
  Stage(sc_module_name nm, sc_fifo<int>& i, sc_fifo<int>& o)
      : sc_module(nm), in(i), out(o) {
    SC_HAS_PROCESS(Stage); SC_THREAD(run);
  }
  void run() {
    for (;;) {
      int v = in.read();
      // TODO(1): 把 v 翻倍后写进 out，并打印 "stage: v -> 2v @ t"
    }
  }
};

SC_MODULE(Sink) {
  sc_fifo<int>& in;
  Sink(sc_module_name nm, sc_fifo<int>& i) : sc_module(nm), in(i) {
    SC_HAS_PROCESS(Sink); SC_THREAD(run);
  }
  void run() {
    for (int i = 0; i < 4; ++i) {
      int v = in.read();
      std::cout << "sink: " << v << " @ " << sc_time_stamp() << std::endl;
    }
  }
};

int sc_main(int, char**) {
  sc_fifo<int> f1(1), f2(1);      // 容量都是 1：强制逐拍流动
  Prod p("p", f1);
  Stage s("s", f1, f2);
  Sink k("k", f2);
  sc_start(60, SC_NS);   // 时限保护：卡死时仿真到点自动结束
  return 0;
}
/* HINT */ /*
两根容量 1 的 fifo 像节拍器：stage 的 out.write 在 f2 满时会阻塞，
sink 的 read 在空时会阻塞——背压自动调节流水线，谁也不用显式握手。
打印格式（照抄到你的 TODO 里）：
  std::cout << "stage: " << v << " -> " << v * 2 << " @ "
            << sc_time_stamp() << std::endl;
推演首拍：@0s prod 写 1 → stage 读 1 → 写 2 → sink 读 2 → 全链 @0s 完成；
之后每 10ns 一拍。这就是数据流建模的最小骨架——
Tiny Accelerator 的 kernel pipeline 本质上就是它的放大版。
*/
