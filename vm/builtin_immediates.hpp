#ifndef RBX_BUILTIN_IMMEDIATES_HPP
#define RBX_BUILTIN_IMMEDIATES_HPP

namespace rubinius {
  class NilClass : public BuiltinType {
  public:
    static const object_type type = NilType;

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };
  /* NOTE(t1):
   * This looks scary, but it's pretty simple. We're specializing
   * the kind_of when passed NilClass to just test using nil_p().
   * This makes kind_of smarter, letting us use it everywhere for
   * type checks. */
  template <>
    static bool kind_of<NilClass>(OBJECT obj) {
      return obj == Qnil;
    }

  class TrueClass : public BuiltinType {
  public:
    static const object_type type = TrueType;

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };

  /* See t1 */
  template <>
    static bool kind_of<TrueClass>(OBJECT obj) {
      return obj == Qtrue;
    }

  class FalseClass : public BuiltinType {
  public:
    static const object_type type = FalseType;

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };

  /* See t1 */
  template <>
    static bool kind_of<FalseClass>(OBJECT obj) {
      return obj == Qfalse;
    }
}

#endif
