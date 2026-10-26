#include <systemc.h>
SC_MODULE(Driver) {
  sc_out<sc_dt::sc_logic> out;
  bool is_da;
  SC_CTOR(Driver) { SC_THREAD(run); }
  void run() {
    if (is_da) {
      out.write(SC_LOGIC_0);
      wait(10, SC_NS);
      out.write(SC_LOGIC_1);
      wait(10, SC_NS);
      out.write(SC_LOGIC_Z);
      wait(30, SC_NS);
    } else {
      out.write(SC_LOGIC_1);
      wait(15, SC_NS);
      out.write(SC_LOGIC_Z);
      wait(30, SC_NS);
    }
  }
};
SC_MODULE(Watcher) {
  sc_in<sc_dt::sc_logic> wire;
  SC_CTOR(Watcher) {
    SC_METHOD(w);
    sensitive << wire;
    // 不加 dont_initialize：初始化阶段先打印一次线的初值
  }
  void w() { std::cout << "wire = " << wire.read().to_char() << " @ " << sc_time_stamp() << std::endl; }
};
SC_MODULE(Top) {
  sc_signal_resolved wire;
  Driver da, db;
  Watcher w;
  SC_CTOR(Top) : da("da"), db("db"), w("w") {
    da.out(wire);  da.is_da = true;
    db.out(wire);  db.is_da = false;
    w.wire(wire);
  }
};
int sc_main(int, char**) {
  Top top("top");
  sc_start(50, SC_NS);
  return 0;
}
