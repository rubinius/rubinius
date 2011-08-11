#ifndef MEL_GRAMMAR19_HPP
#define MEL_GRAMMAR19_HPP

namespace melbourne {
  namespace grammar19 {
    VALUE string_to_ast(VALUE ptp, VALUE name, VALUE source, VALUE line);
    VALUE file_to_ast(VALUE ptp, const char *f, int fd, int start);
  };  // namespace grammar19
};  // namespace melbourne

#endif
