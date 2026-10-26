#include <systemc.h>
SC_MODULE(Delta) {
  sc_signal<int> s;
  SC_CTOR(Delta) { SC_THREAD(run); }
  void run() {
    s.write(42);
    std::cout << "read now: " << s.read() << std::endl;
    wait(SC_ZERO_TIME);
    std::cout << "read after delta: " << s.read() << std::endl;
  }
};
int sc_main(int, char**) {
  Delta d("d");
  sc_start();
  return 0;
}
