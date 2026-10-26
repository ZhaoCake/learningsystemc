// ex11 · 多驱动与 resolved 信号
// 目标：两个驱动器驱动同一根线，输出线上的值（对照期望）
// 知识点：sc_signal 不允许多驱动 / sc_signal_resolved 解析表（X 冲突、Z 释放）
// 本关初始代码【能编译但运行会报错】——读懂报错就是这一关的一半
#include <systemc.h>

SC_MODULE(Driver) {
  sc_out<sc_dt::sc_logic> out;   // resolved 信号上端口类型要用 sc_logic
  SC_CTOR(Driver) { SC_THREAD(run); }
  void run() {
    // TODO(2): 按下面的时间表写线（注意值是 sc_logic 的 X/0/1/Z）
    //   da: 0ns 写 '0'，10ns 写 '1'，20ns 写 'Z'（释放总线），40ns 后保持
    //   db: 0ns 写 '1'，15ns 写 'Z'（释放总线），之后保持
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
  // TODO(1): 把 wire 的类型从 sc_signal<bool> 换成支持多驱动的 resolved 信号
  sc_signal<bool> wire;
  Driver da, db;
  Watcher w;
  SC_CTOR(Top) : da("da"), db("db"), w("w") {
    da.out(wire);
    db.out(wire);
    w.wire(wire);
  }
};

int sc_main(int, char**) {
  Top top("top");
  sc_start(50, SC_NS);
  return 0;
}
/* HINT */ /*
第一步：运行报 multiple writers 错误——普通 sc_signal<bool> 只允许一个驱动，
多驱动要用 sc_signal_resolved（TODO(1)）。
第二步：resolved 的解析表（背下来）：
  相同值 → 该值；{0,1} 冲突 → X；{值,Z} → 值；{Z,Z} → Z
所以 da/db 的端口类型是 sc_dt::sc_logic，写值用 SC_LOGIC_0/1/Z
（或 sc_dt::sc_logic('0') 等），读值打印用 .to_char()。
TODO(2) 的时间表写好后，推一遍解析结果再对照期望：
  0ns: {0,1}→X   10ns: {1,1}→1   15ns: {1,Z}→1（值没变，不触发！）
  20ns: {Z,Z}→Z
注意 15ns 那次：解析值没变，watcher 不会醒——期望里没有 15ns 这一行。
*/
