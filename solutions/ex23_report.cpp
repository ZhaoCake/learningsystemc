#include <systemc.h>
const char* sev_name(sc_severity s) {
  switch (s) {
    case SC_INFO:    return "INFO";
    case SC_WARNING: return "WARNING";
    case SC_ERROR:   return "ERROR";
    case SC_FATAL:   return "FATAL";
  }
  return "?";
}
void my_handler(const sc_report& r, const sc_actions&) {
  std::cout << "[" << sev_name(r.get_severity()) << "] "
            << r.get_msg_type() << ": " << r.get_msg() << std::endl;
}
int sc_main(int, char**) {
  sc_report_handler::set_handler(&my_handler);
  SC_REPORT_INFO("boot", "system online");
  SC_REPORT_WARNING("boot", "low voltage");
  return 0;
}
