#ifndef RBX_KCODE_HPP
#define RBX_KCODE_HPP

namespace rubinius {
namespace kcode {

  typedef const uint8_t table;

  enum CodePage {
    eAscii, eEUC, eSJIS, eUTF8
  };

  static inline bool mbchar_p(table* tbl, uint8_t c) {
    return tbl[c] != 0;
  }

  static inline size_t mbclen(table* tbl, uint8_t c) {
    return tbl[c] + 1;
  }

  void init(STATE);
  table* null_table();
  void set(STATE, CodePage page);

}}

#endif
