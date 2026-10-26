// ex04 · 端口绑定
// 目标：让 Consumer 打印三行 got
// 知识点：sc_out / sc_in / sc_signal / 端口绑定
#include <systemc.h>

SC_MODULE(Producer) {
  sc_out<int> out;
  SC_CTOR(Producer) { SC_THREAD(run); }
  void run() {
    int v = 1;
    for (int i = 0; i < 3; ++i) {
      out.write(v);
      wait(10, SC_NS);
      v *= 2;
    }
  }
};

SC_MODULE(Consumer) {
  sc_in<int> in;
  SC_CTOR(Consumer) {
    SC_METHOD(show);
    sensitive << in;
    dont_initialize();          // 跳过 t=0 的初始化执行，等第一个值变化
  }
  void show() { std::cout << "got " << in.read() << " @ " << sc_time_stamp() << std::endl; }
};

SC_MODULE(Top) {
  sc_signal<int> link;
  Producer prod;
  Consumer cons;
  SC_CTOR(Top) : prod("prod"), cons("cons") {
    // TODO(1): 把 prod 的 out 端口绑到 link
    // TODO(2): 把 cons 的 in 端口绑到 link
  }
};

int sc_main(int, char**) {
  Top top("top");
  sc_start();
  return 0;
}
/* HINT */ /*
端口不能悬空：不绑定，elaboration（细化）阶段直接报错。
绑定就是简单的对象连接：prod.out.bind(link) 或运算符写法 prod.out(link)。
两个方向都连到同一根 sc_signal 上，值的变化就会从生产者流到消费者。
*/
