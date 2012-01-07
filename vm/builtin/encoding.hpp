#ifndef RBX_ENCODING_HPP
#define RBX_ENCODING_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

// See comment in regexp.hpp
#ifndef ONIGURUMA_H
struct OnigEncodingType;
#endif

namespace rubinius {
  class Array;
  class LookupTable;
  class Symbol;
  class String;

  class Encoding : public Object {
  public:
    const static object_type type = EncodingType;

    enum Index {
      eAscii = 0,
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
    static LookupTable* encoding_map(STATE);
    static Array* encoding_list(STATE);
    static void add_constant(STATE, const char* name, Encoding* enc);

    static Encoding* usascii_encoding(STATE);
    static Encoding* utf8_encoding(STATE);
    static Encoding* ascii8bit_encoding(STATE);

    static Encoding* create_bootstrap(STATE, const char* name,
                                      Index index, OnigEncodingType* enc);
    static void create_internal(STATE, const char* name, int index);
    static Encoding* create(STATE, OnigEncodingType* enc, Object* dummy = Qfalse);

    static Encoding* define(STATE, const char* name, OnigEncodingType* enc,
                            Object* dummy = Qfalse);
    static Encoding* define_dummy(STATE, const char* name);
    static Encoding* replicate(STATE, const char* name, const char* original);
    static Encoding* alias(STATE, const char* name, const char* original);

    static Encoding* from_index(STATE, int index);
    static int find_index(STATE, const char* name);
    static Encoding* find(STATE, const char* name);

    OnigEncodingType* get_encoding() {
      return encoding_;
    }

    // Rubinius.primitive :encoding_replicate
    Encoding* replicate(STATE, String* name);

    // Rubinius.primitive :encoding_ascii_compatible_p
    Object* ascii_compatible_p(STATE);

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
      virtual void show(STATE, Object* self, int level);
    };
  };
}

#endif
