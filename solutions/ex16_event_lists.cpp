#include <systemc.h>
SC_MODULE(Barriers) {
  sc_event e1, e2;
  SC_CTOR(Barriers) {
    SC_THREAD(any_waiter);
    SC_THREAD(all_waiter);
    SC_THREAD(driver);
  }
  void any_waiter() {
    sc_event_or_list any = e1 | e2;
    wait(any);
    std::cout << "any fired @ " << sc_time_stamp() << std::endl;
  }
  void all_waiter() {
    sc_event_and_list all = e1 & e2;
    wait(all);
    std::cout << "all fired @ " << sc_time_stamp() << std::endl;
  }
  void driver() {
    e2.notify(5, SC_NS);
    e1.notify(10, SC_NS);
  }
};
int sc_main(int, char**) {
  Barriers b("b");
  sc_start(30, SC_NS);
  return 0;
}
