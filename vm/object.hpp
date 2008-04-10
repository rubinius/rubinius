#ifndef RBX_OOP_H
#define RBX_OOP_H

#include <stdint.h>
#include <sys/types.h>
#include <cstring>
#include <cassert>
#include <vector>

#include "prelude.hpp"

namespace rubinius {

  /* OOP layout:
   * [30 (or 62) bits of data | 2 bits of tag]
   * if tag == 00, the whole thing is a pointer to a memory location.
   * if tag == 01, the data is a fixnum
   * if tag == 10, the data is a literal
   * if tag == 11, the data is any data, using the DATA_* macros 
   */

#define TAG_MASK    0x3
#define TAG_SHIFT   2

#define TAG_REF     0x0
#define TAG_FIXNUM  0x1
#define TAG_LITERAL 0x2
#define TAG_DATA    0x3

#define TAG(v) (((intptr_t)v) & TAG_MASK)
#define APPLY_TAG(v, tag) ((OBJECT)(((intptr_t)v << TAG_SHIFT) | tag))
#define STRIP_TAG(v) (((intptr_t)v) >> TAG_SHIFT)

#define DATA_P(v) (TAG(v) == TAG_DATA)
#define FIXNUM_P(v) (TAG(v) == TAG_FIXNUM)

#define DATA_MASK   0x7
#define DATA_SHIFT  3

#define DATA_TAG_SYMBOL 0x3
#define DATA_TAG_CUSTOM 0x7

#define DATA_TAG(v) ((intptr_t)(v) & DATA_MASK)
#define DATA_APPLY_TAG(v, tag) (OBJECT)((v << DATA_SHIFT) | tag)
#define DATA_STRIP_TAG(v) (((intptr_t)v) >> DATA_SHIFT)

#define SYMBOL_P(v) (DATA_TAG(v) == DATA_TAG_SYMBOL)
#define CUSTOM_P(v) (DATA_TAG(v) == DATA_TAG_CUSTOM)

  /* How many bits of data are available in fixnum, not including
     the sign. */
#define FIXNUM_WIDTH ((8 * sizeof(native_int)) - TAG_SHIFT - 1)
#define FIXNUM_MAX (((native_int)1 << FIXNUM_WIDTH) - 1)
#define FIXNUM_MIN (-(FIXNUM_MAX) - 1)

  /* Standard Rubinius Representation

     Bit layout of special literals:

 6:false    110   % 6 = 6
14:nil     1110   % 6 = 6
10:true    1010   % 6 = 2
18:undef  10010   % 6 = 2


false and nil share the same base bit pattern, allowing RTEST
to be a simple test for that bit pattern.
*/

#define Qfalse ((OBJECT)6L)
#define Qnil   ((OBJECT)14L)
#define Qtrue  ((OBJECT)10L)
#define Qundef ((OBJECT)18L)

#define FALSE_P(v) ((OBJECT)(v) == (OBJECT)Qfalse)
#define TRUE_P(v) ((OBJECT)(v) == (OBJECT)Qtrue)
#define NIL_P(v) ((OBJECT)(v) == (OBJECT)Qnil)
#define UNDEF_P(v) ((OBJECT)(v) == (OBJECT)Qundef)
#define RTEST(v) (((uintptr_t)(v) & 0x7) != 0x6)

#define REFERENCE_P(v) (TAG(v) == TAG_REF)

#define REFERENCE2_P(v) (v && REFERENCE_P(v))

#define INDEXED(obj) (REFERENCE_P(obj) && !obj->StoresBytes)

#define CLASS_OBJECT(obj) (obj->klass)
#define SIZE_OF_OBJECT ((size_t)(sizeof(OBJECT)))

#define NUM_FIELDS(obj)                 (obj->field_count)
#define SET_NUM_FIELDS(obj, fel)        (obj->field_count = fel)
#define SIZE_IN_BYTES_FIELDS(fel)       ((size_t)(sizeof(Object) + \
      (fel*SIZE_OF_OBJECT)))
#define SIZE_IN_WORDS_FIELDS(fel)       (sizeof(Object)/SIZE_OF_OBJECT + fel)
#define SIZE_IN_BYTES(obj)              SIZE_IN_BYTES_FIELDS(obj->field_count)
#define SIZE_OF_BODY(obj)               (obj->field_count * SIZE_OF_OBJECT)
#define ADDRESS_OF_FIELD(obj, fel)      (&obj->field[fel])
#define NTH_FIELD_DIRECT(obj, fel)      (obj->field[fel])
#define SET_FIELD_DIRECT(obj, fel, val) (obj->field[fel] = val)
#define BYTES_OF(obj)                   ((char*)obj->field)


  /* rubinius_object types, takes up 3 bits */
  typedef enum
  {
    ObjectType      = 0,
    MContextType    ,
    BContextType    ,
    ClassType       ,
    MetaclassType   ,
    MTType          ,
    WrapsStructType ,
    IncModType      ,
    TaskType        ,
    FixnumType      ,
    BignumType      ,
    FloatType       ,
    MemPtrType      ,
    StringType      ,
    SymbolType      ,
    CMethodType     ,
    NMethodType     ,
    NilType         ,
    BlockEnvType    ,
    TupleType       ,
    ArrayType       ,
    HashType        ,
    RegexpDataType  ,
    RegexpType      ,
    ByteArrayType   ,
    ISeqType        ,
    ChannelType     ,
    ThreadType      ,
    ModuleType      ,
    SendSiteType    ,
    SelectorType    ,
    LookupTableType ,

    LastObjectType   // must remain at end
  } object_type;

#define object_type_to_type(object_type, type) do {\
  switch((object_type)) { \
  case ObjectType: \
                   type = "Object"; \
    break; \
  case MContextType: \
                     type = "MethodContext"; \
    break; \
  case BContextType: \
                     type = "BlockContext"; \
    break; \
  case ClassType: \
                  type = "Class"; \
    break; \
  case MetaclassType: \
                      type = "Metaclass"; \
    break; \
  case MTType: \
               type = "MethodTable"; \
    break; \
  case WrapsStructType: \
                        type = "SubtendCStructure"; \
    break; \
  case IncModType: \
                   type = "included Module"; \
    break; \
  case TaskType: \
                 type = "Task"; \
    break; \
  case FixnumType: \
                   type = "Fixnum"; \
    break; \
  case BignumType: \
                   type = "Bignum"; \
    break; \
  case FloatType: \
                  type = "Float"; \
    break; \
  case MemPtrType: \
                   type = "MemoryPointer"; \
    break; \
  case StringType: \
                   type = "String"; \
    break; \
  case SymbolType: \
                   type = "Symbol"; \
    break; \
  case CMethodType: \
                    type = "CompiledMethod"; \
    break; \
  case NMethodType: \
                    type = "NativeMethod"; \
    break; \
  case NilType: \
                type = "nil"; \
    break; \
  case LookupTableType: \
                        type = "LookupTable"; \
    break; \
  default: \
           type = "unknown"; \
    break; \
  }} while (0)

  /* rubinius_object gc zone, takes up two bits */
  typedef enum
  {
    UnspecifiedZone  = 0,
    MatureObjectZone = 1,
    YoungObjectZone  = 2,
    LargeObjectZone  = 3,
  } gc_zone;

  typedef size_t hashval;

  /* the sizeof(struct rubinius_object) must an increment of the platform 
     pointer size, so that the bytes located directly after a
     struct rubinius_object can hold a pointer which can be
     dereferenced. (an 32 bit platforms, pointers must be aligned
     on 32bit (word) boundaries. on 64 bit platforms, pointers probably
     have to be aligned on 64bit (double word) boundaries) */

  /* On a 32 bit platform, I expect rubinius_object to take up
     4 + 4 + 4 = 12 bytes.
     on 64 bit platform,
     4 + 4 + 8 = 16 bytes.
     */
  class Object {
  public:
    union {
      struct {
        object_type     obj_type    : 6;
        gc_zone         zone        : 2;
        unsigned int    age         : 3;

        unsigned int Forwarded              : 1;
        unsigned int Remember               : 1;
        unsigned int Marked                 : 1;
        unsigned int ForeverYoung           : 1;
        unsigned int CanStoreIvars          : 1;
        unsigned int StoresBytes            : 1;
        unsigned int RequiresCleanup        : 1;
        unsigned int IsBlockContext         : 1;
        unsigned int IsMeta                 : 1;

        unsigned int CTXFast                : 1;
        unsigned int IsTainted              : 1;
        unsigned int IsFrozen               : 1;
        unsigned int IsLittleEndian         : 1;
        unsigned int RefsAreWeak            : 1;
      };
      uint32_t all_flags;
    };
    uint32_t field_count;
    OBJECT klass;

    union {
      OBJECT field[];
      char bytes[];
    };

    static OBJECT i2n(native_int num) {
      if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
        assert(0);
      } else {
        return APPLY_TAG(num, TAG_FIXNUM);
      }
    }

    static OBJECT i2n(STATE, native_int num);
    static OBJECT ui2n(STATE, unsigned int num);
    static OBJECT ll2n(STATE, long long num);
    static OBJECT ull2n(STATE, unsigned long long num);

    native_int n2i() {
      return STRIP_TAG(this);
    }

    bool fixnum_p() {
      return FIXNUM_P(this);
    }

    bool symbol_p() {
      return SYMBOL_P(this);
    }

    /* Initialize the objects data with the most basic info. This is done
     * right after an object is created. */
    void init(gc_zone loc, size_t fields) {
      all_flags = 0;
      zone = loc;
      field_count = fields;
    }

    /* Clear the body of the object, by setting each field to Qnil */
    void clear_fields() {
      for(size_t i = 0; i < field_count; i++) {
        field[i] = Qnil;
      }
    }

    /* Initialize the object as storing bytes, by setting the flag then clearing the 
     * body of the object, by setting the entire body as bytes to 0 */
    void init_bytes() {
      this->StoresBytes = 1;
      std::memset((void*)(this->field), field_count * sizeof(OBJECT), 0);
    }

    size_t size_in_bytes() {
      return SIZE_IN_BYTES(this);
    }

    bool reference_p() {
      return REFERENCE_P(this);
    }

    bool stores_bytes_p() {
      return StoresBytes;
    }

    bool stores_references_p() {
      return !StoresBytes;
    }

    bool young_object_p() {
      return zone == YoungObjectZone;
    }

    bool mature_object_p() {
      return zone == MatureObjectZone;
    }

    OBJECT at(size_t index) {
      return field[index];
    }

    bool forwarded_p() {
      return Forwarded == 1;
    }

    void set_forward(OBJECT fwd) {
      Forwarded = 1;
      klass = fwd;
    }

    OBJECT forward() {
      return klass;
    }

    bool marked_p() {
      return Marked == 1;
    }

    void mark() {
      Marked = 1;
    }

    void clear_mark() {
      Marked = 0;
    }

    bool nil_p() {
      return this == Qnil;
    }

    bool undef_p() {
      return this == Qundef;
    }

    bool true_p() {
      return this == Qtrue;
    }

    bool false_p() {
      return this == Qfalse;
    }

    bool has_ivars_p() {
      return CanStoreIvars == TRUE;
    }

    bool check_type(object_type type) {
      return reference_p() && obj_type == type;
    }

    bool kind_of_p(OBJECT cls);
    OBJECT class_object();
    OBJECT dup(STATE);
    hashval hash(STATE);
    uintptr_t id(STATE);
    OBJECT metaclass(STATE);

    OBJECT get_ivar(STATE, OBJECT sym);
    OBJECT set_ivar(STATE, OBJECT sym, OBJECT val);
  };

  class VMException { };
  class Assertion : public VMException {
  public:
    char *reason;

    Assertion(char* reason) : reason(reason) { };
  };

  class TypeError : public VMException {
  public:
    object_type type;
    OBJECT object;
    char* reason;

    TypeError(object_type type, OBJECT obj, char* reason = NULL)
      : type(type), object(obj), reason(reason) { };
  };

  /* Used when casting between object types */
  template <class T>
    T* as(OBJECT obj) {
      if(obj->check_type(T::type)) return (T*)obj;
      throw new TypeError(T::type, obj);
    }

  void type_assert(OBJECT obj, object_type type, char* reason);
#define sassert(cond) if(!(cond)) throw new Assertion(#cond);

  /* 
   * A rubinius object can be followed by:
   * - a series of fields, possibly including an ivar
   * - a series of bytes (ByteArray)
   * - a fast_context pointer
   */

  /* Object access, lowest level. These read and set fields of an OBJECT
   * directly. They're built on to integrate with the GC properly. */


  static inline void object_copy_nongc_flags(OBJECT target, OBJECT source)
  {
    target->obj_type        = source->obj_type;
    target->CanStoreIvars   = source->CanStoreIvars;
    target->StoresBytes     = source->StoresBytes;
    target->RequiresCleanup = source->RequiresCleanup;
    target->IsBlockContext  = source->IsBlockContext;
    target->IsMeta          = source->IsMeta;
  }

#define CLEAR_FLAGS(obj)     (obj)->all_flags = 0
#define stack_context_p(obj) ((obj)->gc_zone == UnspecifiedZone)
#define SET_FORWARDED(obj)   (obj)->Forwarded = TRUE
#define FORWARDED_P(obj)     ((obj)->Forwarded)

#define AGE(obj)           (obj->copy_count)
#define CLEAR_AGE(obj)     (obj->copy_count = 0)
#define INCREMENT_AGE(obj) (obj->copy_count++)

  /* Object access. */

  /* Setting the class of an object */
#define rbs_set_class(om, obj, cls) ({ \
    OBJECT _o = (obj), _c = (cls); \
    RUN_WB2(om, _o, _c); _o->klass = _c; })

#define SET_CLASS(o,v) o->set_class(state->om, v)

  /* Accessing and assigning the fields of an object */

#define SET_FIELD(obj, fel, val) rbs_set_field(state->om, obj, fel, val)
#define NTH_FIELD(obj, fel) rbs_get_field(obj, fel)

#define RUN_WB(obj, val) RUN_WB2(state->om, obj, val)
#define RUN_WB2(om, obj, val) if(REFERENCE_P(val) && (obj->gc_zone < val->gc_zone)) object_memory_update_rs(om, obj, val)

  /* No bounds checking! Be careful! */
#define fast_fetch(obj, idx) NTH_FIELD_DIRECT(obj, idx)

  /* Only ever call this with constant arguments! */
#define fast_set(obj, idx, val) ({ \
    RUN_WB(obj, val); \
    SET_FIELD_DIRECT(obj, idx, val); \
    val; \
    })

#define fast_unsafe_set(obj, idx, val) SET_FIELD_DIRECT(obj, idx, val)

#define fast_set_int(obj, idx, int) fast_unsafe_set(obj, idx, I2N(int))
#define fast_inc(obj, idx) fast_unsafe_set(obj, idx, (void*)((uintptr_t)fast_fetch(obj, idx) + (1 << TAG_SHIFT)))

#define EXTRA_PROTECTION 0
#define DISABLE_CHECKS 1

#if DISABLE_CHECKS

#define rbs_set_field(om, obj, fel, val) ({ \
    OBJECT _v = (val), _o = (obj); \
    RUN_WB2(om, _o, _v); \
    SET_FIELD_DIRECT(_o, fel, _v); })

#define rbs_get_field(obj, fel) NTH_FIELD_DIRECT(obj, fel)

#else /*DISABLE_CHECKS*/

  static void _bad_reference2(OBJECT in, int fel) {
    printf("Attempted to access field %d in an object with %lu fields.\n", 
        fel, (unsigned long)NUM_FIELDS(in));

    if(current_machine->g_use_firesuit) {
      machine_handle_fire(FIRE_ACCESS);
    }
  }

#if EXTRA_PROTECTION

  /*
     static void _bad_reference(OBJECT in) {
     printf("Attempted to access field of non-reference.\n");
     if(current_machine->g_use_firesuit) {
     machine_handle_fire(FIRE_NULL);
     }
     }
     */

#define rbs_set_field(om, obj, fel, val) ({ \
    OBJECT _v = (val), _o = (obj); \
    unsigned int _fel = (unsigned int)(fel); \
    if(!REFERENCE_P(_o)) _bad_reference(_o); \
    if(_fel >= _o->field_count) _bad_reference2(_o, _fel); \
    RUN_WB2(om, _o, _v); \
    SET_FIELD_DIRECT(_o, _fel, _v); })

#define rbs_get_field(i_in, i_fel) ({ \
    OBJECT in = (i_in); unsigned int fel = (unsigned int)(i_fel); \
    if(!REFERENCE_P(in)) _bad_reference(in); \
    if(fel >= in->field_count) _bad_reference2(in, fel); \
    NTH_FIELD_DIRECT(in, fel); })

#else /*EXTRA_PROTECTION*/

  /* These are the typically used versions. The don't check for ref, they
     the segfault handler do that. */

#define rbs_set_field(om, obj, fel, val) ({ \
    OBJECT _v = (val), _o = (obj); \
    unsigned int _fel = (unsigned int)(fel); \
    if(_fel >= _o->field_count) _bad_reference2(_o, _fel); \
    RUN_WB2(om, _o, _v); \
    SET_FIELD_DIRECT(_o, _fel, _v); })

#define rbs_get_field(obj, fel) ({ \
    OBJECT _o = (obj); \
    unsigned int _fel = (unsigned int)(fel); \
    if(_fel >= _obj->field_count) _bad_reference2(_o, _fel); \
    NTH_FIELD_DIRECT(_o, _fel); })


#endif /*EXTRA_PROTECTION*/

#endif /*DISABLE_CHECKS*/

  /* A kind of odd one. Use this when assigning an OBJECT into a 'managed' 
   * structed, ie a struct that is contained in the rubinius heap. This 
   * allows the GC to run the write barrier and keep things sane. */
#define SET_STRUCT_FIELD(obj, fel, val) ({ OBJECT _o = (obj), _tmp = (val); RUN_WB(_o, _tmp); fel = _tmp; _tmp; })  

  /* Return the OBJECT for the class of +obj+. Works for all OBJECTs. */
#define _real_class(state, obj) (REFERENCE_P(obj) ? obj->klass : object_class(state, obj))

  /* Type tests. */
#define RTYPE(obj,type) (REFERENCE_P(obj) && obj->obj_type == type)
#define RISA(obj,cls) (REFERENCE_P(obj) && ISA(obj,BASIC_CLASS(cls)))

#define BIGNUM_P(obj) (RTYPE(obj, BignumType))
#define FLOAT_P(obj) (RTYPE(obj, FloatType))
#define COMPLEX_P(obj) (FALSE)

#define INTEGER_P(obj) (FIXNUM_P(obj) || BIGNUM_P(obj))
#define NUMERIC_P(obj) (FIXNUM_P(obj) || COMPLEX_P(obj) || BIGNUM_P(obj) || FLOAT_P(obj))

#define CLASS_P(obj) RTYPE(obj, ClassType)
#define TUPLE_P(obj) RTYPE(obj, TupleType)
#define IO_P(obj) RISA(obj, io)
#define STRING_P(obj) RTYPE(obj, StringType)
#define LOOKUPTABLE_P(obj) RISA(obj, lookuptable)
#define METHODTABLE_P(obj) RTYPE(obj, MTType)
#define ARRAY_P(obj) RTYPE(obj, ArrayType)

#define STRING_OR_NIL_P(obj) (STRING_P(obj) || NIL_P(obj))

#define CMETHOD_P(obj) RTYPE(obj, CMethodType)
#define REGEXP_P(obj) RTYPE(obj, RegexpType)

#define CTX_P(obj) RISA(obj, fastctx)
#define BYTEARRAY_P(obj) RTYPE(obj, ByteArrayType)
#define ISEQ_P(obj) RTYPE(obj, ISeqType)
#define TASK_P(obj) RTYPE(obj, TaskType)
#define CHANNEL_P(obj) RTYPE(obj, ChannelType)
#define BLOCKENV_P(obj) RTYPE(obj, BlockEnvType)
#define THREAD_P(obj) RTYPE(obj, ThreadType)
#define SENDSITE_P(obj) RTYPE(obj, SendSiteType)
#define SELECTOR_P(obj) RTYPE(obj, SelectorType)
#define HASH_P(obj) (RISA(obj, hash))
#define MODULE_P(obj) (RTYPE(obj, ModuleType))
#define POINTER_P(obj) RTYPE(obj, MemPtrType)

  /* HACK: refactor this to use the state_setup_type code path. */
  struct wraps_struct {
    void *ptr;
    void (*mark)(void*);
    void (*free)(void*);
  };

#define MARK_WRAPPED_STRUCT(obj) do { \
  struct wraps_struct *s = (struct wraps_struct *)BYTES_OF(obj); \
  if(s->mark != NULL) { s->mark(s->ptr); } \
} while (0)

#define FREE_WRAPPED_STRUCT(obj) do { \
  struct wraps_struct *s = (struct wraps_struct *)BYTES_OF(obj); \
  if(s->free != NULL) { s->free(s->ptr); } \
} while (0)

};

#endif
