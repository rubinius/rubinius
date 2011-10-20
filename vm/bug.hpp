#ifndef RBX_BUG_HPP
#define RBX_BUG_HPP

namespace rubinius {
  void abort();
  void bug(const char* message);
  void bug(const char* message, const char* arg);
  void warn(const char* message);
  void print_backtrace(size_t max=100);
}

#endif
