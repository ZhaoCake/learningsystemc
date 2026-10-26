// ex23 · 报告机制
// 目标：复现期望输出（自定义报告处理器）
// 知识点：SC_REPORT_INFO/WARNING / sc_report_handler 自定义 handler
#include <systemc.h>

void my_handler(const sc_report& r, const sc_actions&) {
  // TODO(1): 按格式打印：[严重级别] 消息内容
  //   严重级别名：sc_report::get_severity_name(r.get_severity())
  //   消息内容：r.get_msg()
}

int sc_main(int, char**) {
  // TODO(2): 安装自定义 handler
  SC_REPORT_INFO("boot", "system online");
  SC_REPORT_WARNING("boot", "low voltage");
  return 0;
}
/* HINT */ /*
SC_REPORT_INFO(type, msg) 是 SystemC 的统一诊断出口（比 cout 好在：
可过滤、可重定向、可统计）。默认 handler 的输出格式你早见过——
仿真结束那段版权信息就是它打的。
set_handler(&my_handler) 之后所有报告都经你的手。
严重级别从低到高：INFO / WARNING / ERROR / FATAL，
FATAL 默认直接终止仿真——错误处理要分级。
*/
