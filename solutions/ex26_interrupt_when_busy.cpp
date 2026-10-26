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
      if (!irq_pending) {
        std::cout << "arm irq #" << n + 1 << " @ " << sc_time_stamp() << std::endl;
        wait(irq);
      }
      irq_pending = false;
      n++;
      std::cout << "handle irq #" << n << " @ " << sc_time_stamp() << std::endl;
      wait(5, SC_NS);
    }
  }
  void handler() {
    wait(8, SC_NS);  irq.notify();
    wait(2, SC_NS);  irq.notify(); irq_pending = true;
    wait(20, SC_NS);
  }
};
int sc_main(int, char**) {
  Device d("d");
  sc_start(30, SC_NS);
  return 0;
}
