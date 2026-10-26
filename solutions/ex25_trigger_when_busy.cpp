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
          busy = true;
          std::cout << "start @ " << sc_time_stamp() << std::endl;
          done.notify(10, SC_NS);
        }
      }
      if (done.triggered()) {
        std::cout << "finish @ " << sc_time_stamp() << std::endl;
        busy = false;
        if (pending) {
          pending = false;
          busy = true;
          std::cout << "start @ " << sc_time_stamp() << std::endl;
          done.notify(10, SC_NS);
        }
      }
    }
  }
  void driver() {
    wait(2, SC_NS);  trigger.notify();
    wait(3, SC_NS);  trigger.notify();
    wait(3, SC_NS);  trigger.notify();
    wait(9, SC_NS);  trigger.notify();
    wait(7, SC_NS);  trigger.notify();
    wait(20, SC_NS);
  }
};
int sc_main(int, char**) {
  Worker w("w");
  sc_start(50, SC_NS);
  return 0;
}
