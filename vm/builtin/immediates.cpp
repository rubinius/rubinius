#include "builtin/immediates.hpp"

namespace rubinius {

  /* NOTE(t1):
   * This looks scary, but it's pretty simple. We're specializing
   * the kind_of when passed NilClass to just test using nil_p().
   * This makes kind_of smarter, letting us use it everywhere for
   * type checks. */
  template <>
    bool kind_of<NilClass>(OBJECT obj) {
      return obj == Qnil;
    }

  /* See t1 */
  template <>
    bool kind_of<TrueClass>(OBJECT obj) {
      return obj == Qtrue;
    }

  /* See t1 */
  template <>
    bool kind_of<FalseClass>(OBJECT obj) {
      return obj == Qfalse;
    }

  void FalseClass::Info::mark(OBJECT t, ObjectMark& mark) { }
  void TrueClass::Info::mark(OBJECT t, ObjectMark& mark) { }
  void NilClass::Info::mark(OBJECT t, ObjectMark& mark) { }
}
