#include <systemc.h>
SC_MODULE(Worker) {
  sc_mutex& m;
  const char* tag;
  int delay;
  Worker(sc_module_name nm, sc_mutex& m_, const char* t_, int d_)
      : sc_module(nm), m(m_), tag(t_), delay(d_) { SC_HAS_PROCESS(Worker); SC_THREAD(run); }
  void run() {
    wait(delay, SC_NS);
    m.lock();
    std::cout << tag << " in CS @ " << sc_time_stamp() << std::endl;
    wait(5, SC_NS);
    std::cout << tag << " out CS @ " << sc_time_stamp() << std::endl;
    m.unlock();
  }
};
int sc_main(int, char**) {
  sc_mutex m;
  Worker a("a", m, "A", 0);
  Worker b("b", m, "B", 2);
  sc_start(20, SC_NS);
  return 0;
}
