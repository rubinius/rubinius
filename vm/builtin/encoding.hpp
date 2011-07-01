#ifndef RBX_ENCODING_HPP
#define RBX_ENCODING_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

// See comment in regexp.hpp
#ifndef ONIGURUMA_H
struct OnigEncodingType;
#endif

namespace rubinius {
  class LookupTable;
  class Symbol;
  class String;

  class Encoding : public Object {
  public:
    const static object_type type = EncodingType;

    enum Index {
      eAscii,
      eBinary,
      eUtf8
    };

  private:
    String* name_;    // slot
    Object* dummy_;   // slot

    OnigEncodingType* encoding_;

  public:
    attr_accessor(name, String);
    attr_accessor(dummy, Object);

    static void init(STATE);

    static Class* internal_class(STATE);
    static LookupTable* symbol_map(STATE);
    static LookupTable* index_map(STATE);
    static void add_constant(STATE, const char* name, Encoding* enc);

    static Encoding* ascii_encoding(STATE);
    static Encoding* utf8_encoding(STATE);

    static Encoding* create_bootstrap(STATE, const char* name,
                                      Index index, OnigEncodingType* enc);
    static Encoding* create(STATE, OnigEncodingType* enc, Object* dummy = Qfalse);

    static Encoding* define(STATE, const char* name, OnigEncodingType* enc,
                            Object* dummy = Qfalse);
    static Encoding* alias(STATE, const char* name, Encoding* enc);

    static Encoding* find(STATE, Symbol* name);

    OnigEncodingType* get_encoding() {
      return encoding_;
    }

    // Rubinius.primitive :encoding_replicate
    Encoding* replicate(STATE, String* name);

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void auto_mark(Object* obj, ObjectMark& mark);
      virtual void auto_visit(Object* obj, ObjectVisitor& visit);
      virtual void set_field(STATE, Object* target, size_t index, Object* val);
      virtual Object* get_field(STATE, Object* target, size_t index);
      virtual void populate_slot_locations();
    };
  };
}

#endif
