#ifndef RBX_OOP_HPP
#define RBX_OOP_HPP

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

#define FALSE_P(v) ((OBJECT)(v) == (OBJECT)Qfalse)
#define TRUE_P(v) ((OBJECT)(v) == (OBJECT)Qtrue)
#define NIL_P(v) ((OBJECT)(v) == (OBJECT)Qnil)
#define UNDEF_P(v) ((OBJECT)(v) == (OBJECT)Qundef)
#define RTEST(v) (((uintptr_t)(v) & 0x7) != 0x6)

#define REFERENCE_P(v) (TAG(v) == TAG_REF)

#define INDEXED(obj) (REFERENCE_P(obj) && !obj->StoresBytes)

#define SIZE_OF_OBJECT ((size_t)(sizeof(OBJECT)))

#define NUM_FIELDS(obj)                 (obj->field_count)
#define SET_NUM_FIELDS(obj, fel)        (obj->field_count = fel)
#define SIZE_IN_BYTES_FIELDS(fel)       ((size_t)(sizeof(Object) + \
      (fel*SIZE_OF_OBJECT)))
#define SIZE_IN_WORDS_FIELDS(fel)       (sizeof(Object)/SIZE_OF_OBJECT + fel)
#define SIZE_IN_BYTES(obj)              SIZE_IN_BYTES_FIELDS(obj->field_count)
#define SIZE_OF_BODY(obj)               (obj->field_count * SIZE_OF_OBJECT)


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
    IOBufferType    ,
    StaticScopeType ,
    SymbolTableType ,
    MatchDataType   ,
    ExceptionType   ,
    IOType          ,
    ExecutableType  ,
    CMVisibilityType,
    ListType        ,

    LastObjectType   // must remain at end
  } object_type;

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

  class ObjectBoundsExceeded : public VMException {
  public:
    OBJECT obj;
    size_t index;

    ObjectBoundsExceeded(OBJECT o, size_t i) : obj(o), index(i) { }
  };

  class Class;
  class MetaClass;
  class Integer;

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
    Class* klass;

    union {
      OBJECT field[];
      char bytes[];
    };

    static FIXNUM i2n(native_int num) {
      if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
        assert(0);
      } else {
        return (FIXNUM)APPLY_TAG(num, TAG_FIXNUM);
      }
    }

    static Integer* i2n(STATE, native_int num);
    static Integer* ui2n(STATE, unsigned int num);
    static Integer* ll2n(STATE, long long num);
    static Integer* ull2n(STATE, unsigned long long num);

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

    size_t body_in_bytes() {
      return field_count * sizeof(OBJECT);
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
      if(field_count <= index) {
        throw new ObjectBoundsExceeded(this, index);
      }
      return field[index];
    }

    bool forwarded_p() {
      return Forwarded == 1;
    }

    void set_forward(OBJECT fwd) {
      assert(zone == YoungObjectZone);
      Forwarded = 1;
      klass = (Class*)fwd;
    }

    OBJECT forward() {
      return (OBJECT)klass;
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

    void cleanup(STATE);

    bool kind_of_p(STATE, OBJECT cls);
    Class* lookup_begin(STATE);
    Class* class_object(STATE);
    OBJECT dup(STATE);
    hashval hash(STATE);
    uintptr_t id(STATE);
    Class* metaclass(STATE);

    OBJECT get_ivar(STATE, OBJECT sym);
    OBJECT set_ivar(STATE, OBJECT sym, OBJECT val);

    static const char* type_to_name(object_type type);
  };


  void inspect(STATE, OBJECT);
  void inspect(STATE, SYMBOL);

  /* Given builtin-class +T+, return true if +obj+ is of class +T+ */
  template <class T>
    static bool kind_of(OBJECT obj) {
      if(obj->reference_p()) {
        return obj->obj_type == T::type;
      }
      return false;
    }

  /* Used when casting between object types.
   *
   * Given builtin class +T+, return +obj+ cast as type +T*+. If
   * +obj+ is not of type +T+, throw's a TypeError exception.
   * */
  template <class T>
    static T* as(OBJECT obj) {
      /* The 'obj &&' gives us additional saftey, checking for
       * NULL objects. */
      if(obj && kind_of<T>(obj)) return (T*)obj;
      throw new TypeError(T::type, obj);
    }

  /* Similar to as<>, but returns NULL if the type is invalid. ONLY
   * use this when doing a conditional cast. */
  template <class T>
    static T* try_as(OBJECT obj) {
      /* The 'obj &&' gives us additional saftey, checking for
       * NULL objects. */
      if(obj && kind_of<T>(obj)) return (T*)obj;
      return NULL;
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

#define CLEAR_FLAGS(obj)     (obj)->all_flags = 0
#define stack_context_p(obj) ((obj)->gc_zone == UnspecifiedZone)
#define SET_FORWARDED(obj)   (obj)->Forwarded = TRUE
#define FORWARDED_P(obj)     ((obj)->Forwarded)

#define AGE(obj)           (obj->copy_count)
#define CLEAR_AGE(obj)     (obj->copy_count = 0)
#define INCREMENT_AGE(obj) (obj->copy_count++)

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
