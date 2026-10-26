// ex26 · 模式：中断不丢（interrupt pending）
// 目标：复现期望输出
// 知识点：事件通知会丢 / 硬件式 pending 标志补救
#include <systemc.h>

SC_MODULE(Device) {
  sc_event irq;
  bool irq_pending = false;
  SC_CTOR(Device) {
    SC_HAS_PROCESS(Device);
    SC_THREAD(waiter);
    SC_THREAD(handler);
  }
  void waiter() {
    int n = 0;
    for (;;) {
      // TODO(1): 若无挂起中断：编号 n+1、打印 "arm irq #n+1"、wait(irq) 布防
      // TODO(2): 清除挂起标志；n++；打印 "handle irq #n"；处理耗时 5ns
      //         （注意：pending 路径也要 n++，中断编号不能丢）
      std::cout << "arm irq #" << n + 1 << " @ " << sc_time_stamp() << std::endl;
      wait(irq);
      std::cout << "handle irq #" << n << " @ " << sc_time_stamp() << std::endl;
      wait(5, SC_NS);
    }
  }
  void handler() {
    wait(8, SC_NS);  irq.notify();
    // TODO(3): 2ns 后再来一次中断——此时 waiter 正在处理 #1（忙到 13ns），
    //          单纯 notify 会丢，必须同时置 irq_pending = true
    wait(2, SC_NS);
    wait(20, SC_NS);
  }
};

int sc_main(int, char**) {
  Device d("d");
  sc_start(30, SC_NS);
  return 0;
}
/* HINT */ /*
sc_event.notify() 是"电平脉冲"不是"电平保持"：没人等，通知就蒸发了。
本关 @10ns 的中断恰好撞上 waiter 在处理 #1（wait(5,SC_NS) 中，不敏感）——
裸 notify 必丢。硬件里的解法就是中断挂起寄存器：handler 置 irq_pending=true，
waiter 处理完回循环时先查标志，有挂起就不再布防、直接进处理。
对照期望：#2 没有 arm 行（没布防直接 handle），这就是 pending 的痕迹。
这个模式你以后在 RTL 中断控制器里会天天见到。
*/
