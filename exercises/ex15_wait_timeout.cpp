// ex15 · 带超时的事件等待
// 目标：复现期望输出（两段等待各自何时返回）
// 知识点：wait(timeout, event) —— 事件与超时谁先到听谁的
#include <systemc.h>

SC_MODULE(Timeout) {
  sc_event ev;
  SC_CTOR(Timeout) { SC_THREAD(t); }
  void t() {
    // TODO(1): 让事件在 20ns 时触发
    wait(10, SC_NS, ev);          // 最多等到 10ns
    std::cout << "first wait done @ " << sc_time_stamp() << std::endl;
    wait(15, SC_NS, ev);          // 最多等到 25ns
    std::cout << "second wait done @ " << sc_time_stamp() << std::endl;
  }
};

int sc_main(int, char**) {
  Timeout x("x");
  sc_start(40, SC_NS);
  return 0;
}
/* HINT */ /*
wait(t, ev) 是"事件 or 超时"的竞速：谁先到返回谁，时间戳说明了一切。
第一段：10ns 超时先到（事件要 20ns 才发）→ 10ns 返回。
第二段：从 10ns 再等最多 15ns（上限 25ns），但事件 20ns 就到 → 20ns 返回。
TODO(1) 漏掉的话，第二段只能等到 25ns 超时——对照期望看差异。
这种"等事件但别等死"的写法是验证平台 heartbeat/watchdog 的基础。
*/
