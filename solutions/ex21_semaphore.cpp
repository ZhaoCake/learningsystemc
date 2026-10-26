#include <systemc.h>
SC_MODULE(Worker) {
  sc_semaphore& s;
  const char* tag;
  int delay;
  Worker(sc_module_name nm, sc_semaphore& s_, const char* t_, int d_)
      : sc_module(nm), s(s_), tag(t_), delay(d_) { SC_HAS_PROCESS(Worker); SC_THREAD(run); }
  void run() {
    wait(delay, SC_NS);
    s.wait();
    std::cout << tag << " in @ " << sc_time_stamp() << std::endl;
    wait(10, SC_NS);
    std::cout << tag << " out @ " << sc_time_stamp() << std::endl;
    s.post();
  }
};
int sc_main(int, char**) {
  sc_semaphore sem(2);
  Worker a("a", sem, "A", 0);
  Worker b("b", sem, "B", 1);
  Worker c("c", sem, "C", 2);
  sc_start(30, SC_NS);
  return 0;
}
