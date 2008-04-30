#ifndef RBX_OBJECTS_HPP
#define RBX_OBJECTS_HPP

#include "vm.hpp"
#include "object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class BuiltinType : public Object {

  };

  class NilClass : public BuiltinType { };
  /* NOTE(t1):
   * This looks scary, but it's pretty simple. We're specializing
   * the kind_of when passed NilClass to just test using nil_p().
   * This makes kind_of smarter, letting us use it everywhere for
   * type checks. */
  template <>
    static bool kind_of<NilClass>(OBJECT obj) {
      return obj == Qnil;
    }

  class TrueClass : public BuiltinType { };
  /* See t1 */
  template <>
    static bool kind_of<TrueClass>(OBJECT obj) {
      return obj == Qtrue;
    }

  class FalseClass : public BuiltinType { };
  /* See t1 */
  template <>
    static bool kind_of<FalseClass>(OBJECT obj) {
      return obj == Qfalse;
    }
  
  class Integer : public BuiltinType {
  public:
    inline native_int n2i();
  };
}

#include "builtin_bignum.hpp"

namespace rubinius {

  class Fixnum : public Integer {
  public:
    const static size_t fields = 0;
    const static object_type type = FixnumType;

    INTEGER add(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() + other->n2i());
    }

    INTEGER sub(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() - other->n2i());
    }

    INTEGER multiply(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() * other->n2i());
    }

    INTEGER divide(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() / other->n2i());
    }

    INTEGER modulo(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() % other->n2i());
    }
    
    native_int to_nint() {
      return STRIP_TAG(this);
    }

  };

  typedef Fixnum* FIXNUM;

  /* See t1 */
  template <>
    static bool kind_of<Integer>(OBJECT obj) {
      return obj->fixnum_p() || (obj->reference_p() && obj->obj_type == Bignum::type);
    }

  /* For some reason, the as<> template doesn't pick up the specialized kind_of<>, until
   * we figure out why, just special as<> too. */
  template <>
    static INTEGER as<Integer>(OBJECT obj) {
      if(kind_of<Integer>(obj)) return (Integer*)obj;
      throw new TypeError(obj->obj_type, obj, "can't be cast as an Integer");
    }

  template <>
    static bool kind_of<Fixnum>(OBJECT obj) {
      return obj->fixnum_p();
    }
  
  class StaticScope : public BuiltinType {
    public:
    const static size_t fields = 3;
    const static object_type type = StaticScopeType;

    OBJECT instance_variables;
    Module* module;
    StaticScope* parent;

    static StaticScope* create(STATE);
  };

  native_int Integer::n2i() {
    if(fixnum_p()) {
      return ((FIXNUM)this)->to_nint();
    }

    return as<Bignum>(this)->to_nint();
  }

};

namespace rubinius {

  class String;
  class Hash;
  class Tuple;

  class Symbol : public BuiltinType {
    public:
    const static size_t fields = 0;
    const static object_type type = SymbolType;

    native_int index() {
      return DATA_STRIP_TAG(this);
    }

    static Symbol* from_index(STATE, size_t index) {
      return (Symbol*)DATA_APPLY_TAG(index, DATA_TAG_SYMBOL);
    }

    String* to_str(STATE);
  };

  /* See t1 */
  template <>
    static bool kind_of<Symbol>(OBJECT obj) {
      return obj->symbol_p();
    }

  typedef Symbol* SYMBOL;

  class SymbolTable : public BuiltinType {
    public:
    const static size_t fields = 3;
    const static object_type type = SymbolTableType;

    OBJECT instance_variables;
    Tuple* symbols;
    Hash*  strings;

    static SymbolTable* create(STATE);
    SYMBOL lookup(STATE, const char* str, size_t size = 0);
    SYMBOL lookup(STATE, String* str);
    String* find_string(STATE, Symbol* sym);
  };
};

namespace rubinius {
  class Tuple : public BuiltinType {
    public:
    const static size_t fields = 0;
    const static object_type type = TupleType;

    static Tuple* create(STATE, size_t fields);
    static Tuple* from(STATE, size_t fields, ...);
    static bool is_a(OBJECT obj) {
      return obj->obj_type == TupleType;
    }

    OBJECT put(STATE, size_t idx, OBJECT val);
    void copy_from(STATE, Tuple* other, int start, int end);

  };
};

#include "builtin_hash.hpp"


namespace rubinius {
  class NormalObject : public BuiltinType {
    public:
    const static size_t fields = 1;
    const static object_type type = ObjectType;

    OBJECT instance_variables;
  };
};

#include "builtin_array.hpp"

namespace rubinius {
  class Exception : public BuiltinType {
    public:
    const static size_t fields = 3;
    const static object_type type = ExceptionType;

    static Exception* create(STATE);
    OBJECT instance_variables;
    OBJECT message;
    OBJECT context;
  };
};

namespace rubinius {
  class CompiledMethod;
  class MethodContext;
  class BlockContext;

  class BlockEnvironment : public BuiltinType {
    public:
    const static size_t fields = 5;
    const static object_type type = BlockEnvType;

    OBJECT instance_variables;
    MethodContext* home;
    MethodContext* home_block;
    OBJECT local_count;
    CompiledMethod* method;

    static BlockEnvironment* under_context(STATE, CompiledMethod* cm,
        MethodContext* parent, MethodContext* active);

    void call(STATE, size_t args);
    BlockContext* create_context(STATE);
  };
};


namespace rubinius {
  class IO : public BuiltinType {
    public:
    const static size_t fields = 4;
    const static object_type type = IOType;

    OBJECT instance_variables;
    OBJECT descriptor;
    OBJECT buffer;
    OBJECT mode;

    static void init(STATE);
    static IO* create(STATE, int fd);

    class Buffer : public BuiltinType {
    public:
      const static size_t fields = 4;
      const static object_type type = IOBufferType;

      OBJECT instance_variables;
      OBJECT storage;
      INTEGER total;
      INTEGER used;

      static Buffer* create(STATE, size_t bytes);
      void reset(STATE);
      String* drain(STATE);

      char* byte_address() {
        return (char*)storage->bytes;
      }

      size_t left() {
        return total->n2i() - used->n2i();
      }

      char* at_unused() {
        char* start = (char*)storage->bytes;
        start += used->n2i();
        return start;
      }

      void read_bytes(size_t bytes) {
        used = Object::i2n(used->n2i() + bytes);
      }

    };
  };
};

#include "builtin_regexp.hpp"
#include "builtin_string.hpp"

namespace rubinius {
  class ByteArray : public BuiltinType {
    public:
    const static size_t fields = 0;
    const static object_type type = ByteArrayType;

    static ByteArray* create(STATE, size_t bytes);
  };
};

namespace rubinius {
  #define LOOKUPTABLE_MIN_SIZE 16
  class LookupTable : public BuiltinType {
    public:
    const static size_t fields = 4;
    const static object_type type = LookupTableType;

    OBJECT instance_variables;
    Tuple* values;
    INTEGER bins;
    INTEGER entries;

    /* Prototypes */
    static LookupTable* create(STATE, size_t sz = LOOKUPTABLE_MIN_SIZE);
    void setup(STATE, size_t sz);
    OBJECT store(STATE, OBJECT key, OBJECT val);
    OBJECT fetch(STATE, OBJECT key);
    OBJECT fetch(STATE, OBJECT key, bool* found);
    LookupTable* dup(STATE);
    static OBJECT entry_new(STATE, OBJECT key, OBJECT val);
    static OBJECT entry_append(STATE, OBJECT top, OBJECT nxt);
    void   redistribute(STATE, size_t size);
    OBJECT find_entry(STATE, OBJECT key);
    OBJECT find(STATE, OBJECT key);
    OBJECT remove(STATE, OBJECT key);
    OBJECT has_key(STATE, OBJECT key);
    static Array* collect(STATE, LookupTable* tbl, OBJECT (*action)(STATE, OBJECT));
    static OBJECT get_key(STATE, OBJECT entry);
    Array* all_keys(STATE);
    static OBJECT get_value(STATE, OBJECT entry);
    Array* all_values(STATE);
    static OBJECT get_entry(STATE, OBJECT entry);
    OBJECT all_entries(STATE);
  };

  class MethodTable : public LookupTable {
    public:
    const static object_type type = MTType;
    static MethodTable* create(STATE);
  };
};

namespace rubinius {

  class Executable : public BuiltinType {
    public:
    const static size_t fields = 4;
    const static object_type type = ExecutableType;

    OBJECT instance_variables;
    OBJECT primitive;
    OBJECT required;
    OBJECT serial;
  };


  class ISeq;
  class MemoryPointer;
  class VMMethod;

  class CompiledMethod : public Executable {
    public:
    const static size_t fields = 20;
    const static object_type type = CMethodType;
    const static size_t saved_fields = 16;

    SYMBOL name;
    ISeq*  iseq;
    FIXNUM stack_size;
    FIXNUM local_count;
    FIXNUM required_args;
    FIXNUM total_args;
    OBJECT splat;
    Tuple* literals;
    Tuple* exceptions;
    Tuple* lines;
    SYMBOL file;
    SYMBOL path;
    FIXNUM serial;
    OBJECT bonus;
    MemoryPointer* compiled;
    StaticScope* scope;

    static CompiledMethod* create(STATE);
    void post_marshal(STATE);
    size_t number_of_locals();
    void set_scope(StaticScope*);
    VMMethod* vmmethod(STATE);

    class Visibility : public BuiltinType {
    public:
      const static size_t fields = 3;
      const static object_type type = CMVisibilityType;

      LookupTable* instance_variables;
      SYMBOL visibility;
      Executable* method;

      static Visibility* create(STATE);

      bool public_p(STATE) {
        return visibility == G(sym_public);
      }

      bool private_p(STATE) {
        return visibility == G(sym_private);
      }

      bool protected_p(STATE) {
        return visibility == G(sym_protected);
      }
    };
  };

  template <>
    static bool kind_of<Executable>(OBJECT obj) {
      if(obj->obj_type == Executable::type ||
         obj->obj_type == CompiledMethod::type) {
        return true;
      }

      return false;
    }
};


#include "builtin_class.hpp"
#include "builtin_contexts.hpp"
#include "builtin_iseq.hpp"
#include "builtin_float.hpp"
#include "builtin_list.hpp"
#include "builtin_selector.hpp"
#include "builtin_task.hpp"
#include "builtin_iseq.hpp"
#include "builtin_channel.hpp"

#endif
