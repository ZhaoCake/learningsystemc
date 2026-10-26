// ex25 · 模式：忙时收到触发怎么办
// 目标：复现期望输出（pending 排队 + 干完立即处理积压）
// 知识点：triggered() 区分事件来源 / 忙碌排队模式
#include <systemc.h>

SC_MODULE(Worker) {
  sc_event trigger, done;
  bool busy = false, pending = false;
  SC_CTOR(Worker) {
    SC_HAS_PROCESS(Worker);
    SC_THREAD(run);
    SC_THREAD(driver);
  }
  void run() {
    for (;;) {
      wait(trigger | done);
      if (trigger.triggered()) {
        if (busy) {
          pending = true;
          std::cout << "queued @ " << sc_time_stamp() << std::endl;
        } else {
          // TODO(1): 空闲时接单：置 busy、打印 "start @ t"、安排 10ns 后的 done
        }
      }
      if (done.triggered()) {
        std::cout << "finish @ " << sc_time_stamp() << std::endl;
        busy = false;
        // TODO(2): 有积压就立刻再开工（打印 start 并再次安排 done）
      }
    }
  }
  void driver() {
    wait(2, SC_NS);  trigger.notify();
    wait(3, SC_NS);  trigger.notify();
    wait(3, SC_NS);  trigger.notify();
    wait(9, SC_NS);  trigger.notify();    // @17
    wait(7, SC_NS);  trigger.notify();    // @24
    wait(20, SC_NS);
  }
};

int sc_main(int, char**) {
  Worker w("w");
  sc_start(50, SC_NS);
  return 0;
}
/* HINT */ /*
单事件通知在忙碌时会"丢"——本关用 sc_event_or_list + triggered() 区分来源：
wait(trigger | done) 醒来后，trigger.triggered() 为真说明是来新活了。
排队逻辑：busy 时只置 pending 标志；
收尾逻辑：finish 后如果 pending，立即置回 busy、打印 start、再安排 done——
积压的活不需要等到下一次 trigger。
对照期望：@5 和 @8 两次 trigger 只打一行 queued（标志本来就是 true，
第二次赋值无所谓），@12 finish 后立刻 start@12（重入）。
*/
