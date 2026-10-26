// ex12 · sc_time 算术
// 目标：输出三个时间对象和一个时间戳（对照期望校准单位换算）
// 知识点：sc_time / SC_NS·SC_US·SC_MS / time resolution
#include <systemc.h>

int sc_main(int, char**) {
  sc_time t1(10, SC_NS);
  sc_time t2(1.5, SC_US);
  // TODO(1): 定义 t3 = t1 + t2，打印 t1 t2 t3（一行，空格分隔）
  std::cout << t1 << " " << t2 << std::endl;
  // TODO(2): 用 sc_start(2.5, SC_MS) 推进仿真时间，然后打印时间戳
  // 注意：wait() 只能在 SC_THREAD/SC_CTHREAD 里用，sc_main 里推进时间用 sc_start
  std::cout << sc_time_stamp() << std::endl;
  return 0;
}
/* HINT */ /*
sc_time 支持加减与流输出（<< 直接打印，自动选可读单位）。
1.5 us = 1500 ns，所以 t3 是 1510 ns——打印时内核会自动挑一个
"干净"的单位显示。别手工算字符串，让 operator<< 干活。
2.5 ms 的推进在 sc_main 里要用 sc_start(2.5, SC_MS)——
wait() 只属于进程（SC_THREAD/SC_CTHREAD），这是新手高频编译/运行错误。
*/
