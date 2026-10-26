// ex22 · SystemC 数据类型
// 目标：复现期望输出（注意位宽截断与下标顺序）
// 知识点：sc_uint<W> / sc_int<W> / sc_bv<W> 位操作
#include <systemc.h>

int sc_main(int, char**) {
  sc_uint<8> a = 0xA5;
  std::cout << a << std::endl;                  // 十进制打印
  // TODO(1): 取 a 的高 4 位（右移 4 位），打印结果（十进制）
  sc_bv<4> v("1010");
  // TODO(2): 打印 v 的十六进制形式，以及 v[0] 和 v[3]（紧挨着打印，先 v[0] 后 v[3]）
  return 0;
}
/* HINT */ /*
sc_uint<W> 无符号、sc_int<W> 有符号（补码），位宽固定，溢出按 W 截断——
和 int 的天然混用要小心。0xA5 高 4 位是 0xA = 10。
sc_bv<W> 逐位访问 v[i]：i=0 是最低位（LSB）！字符串 "1010" 左边是 MSB，
所以 v[0]='0'、v[3]='1'。to_string(sc_dt::SC_HEX) 转十六进制字符串。
硬件数据类型位宽精确，综合/验证里一律用它们而不是裸 int。
*/
