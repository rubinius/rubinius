#ifndef RBX_SUBTEND_RUBY_H
#define RBX_SUBTEND_RUBY_H

/**
 *  @file
 *
 *  Notes:
 *
 *    - The function prefix capi_* is used for functions that implement
 *      the Ruby C-API but should NEVER be used in a C extension's code.
 *
 *      Just in case, that means NEVER, like NOT EVER. If you do, we'll
 *      call your mother.
 *
 *  @todo Blocks/iteration. rb_iterate normally uses fptrs, could
 *        maybe do that or then support 'function objects' --rue
 *
 *  @todo RARRAY, RSTRING etc. --rue
 *
 *  @todo Const correctness. --rue
 *
 *  @todo Add some type of checking for too-long C strings etc? --rue
 *
 */

#include <stddef.h>
#include <stdint.h>

#define RUBY

#ifdef __cplusplus
# ifndef  HAVE_PROTOTYPES
#  define HAVE_PROTOTYPES 1
# endif
# ifndef  HAVE_STDARG_PROTOTYPES
#  define HAVE_STDARG_PROTOTYPES 1
# endif
#endif

#undef _
#ifdef HAVE_PROTOTYPES
# define _(args) args
#else
# define _(args) ()
#endif

#undef __
#ifdef HAVE_STDARG_PROTOTYPES
# define __(args) args
#else
# define __(args) ()
#endif

#ifdef __cplusplus
#define ANYARGS ...
#else
#define ANYARGS
#endif

#ifdef __STDC__
# include <limits.h>
#else
# ifndef LONG_MAX
#  ifdef HAVE_LIMITS_H
#   include <limits.h>
#  else
    /* assuming 32bit(2's compliment) long */
#   define LONG_MAX 2147483647
#  endif
# endif
# ifndef LONG_MIN
#  define LONG_MIN (-LONG_MAX-1)
# endif
# ifndef CHAR_BIT
#  define CHAR_BIT 8
# endif
#endif

#ifndef RUBY_EXTERN
#define RUBY_EXTERN extern
#endif

void* XMALLOC(size_t bytes);
void  XFREE(void* ptr);
void* XREALLOC(void* ptr, size_t bytes);
void* XCALLOC(size_t items, size_t bytes);

#define xmalloc   XMALLOC
#define xcalloc   XCALLOC
#define xrealloc  XREALLOC
#define xfree     XFREE

#define ruby_xmalloc   xmalloc
#define ruby_xcalloc   xcalloc
#define ruby_xrealloc  xrealloc
#define ruby_xfree     xfree

/**
 *  In MRI, VALUE represents an object.
 *
 *  In rbx, this is a Handle.
 */
#define VALUE intptr_t

/**
 *  In MRI, ID represents an interned string, i.e. a Symbol.
 *
 *  In rbx, this is a Symbol pointer.
 */
#define ID    intptr_t

/**
 * In MRI, RUBY_DATA_FUNC is used for the mark and free functions in
 * Data_Wrap_Struct and Data_Make_Struct.
 */
typedef void (*RUBY_DATA_FUNC)(void*);

/* "Stash" the real versions. */
#define RBX_Qfalse      (reinterpret_cast<Object*>(0x0aUL))
#define RBX_Qnil        (reinterpret_cast<Object*>(0x1aUL))
#define RBX_Qtrue       (reinterpret_cast<Object*>(0x12UL))
#define RBX_Qundef      (reinterpret_cast<Object*>(0x22UL))

#define RBX_FALSE_P(o)  (reinterpret_cast<Object*>((o)) == RBX_Qfalse)
#define RBX_TRUE_P(o)   (reinterpret_cast<Object*>((o)) == RBX_Qtrue)
#define RBX_NIL_P(o)    (reinterpret_cast<Object*>((o)) == RBX_Qnil)
#define RBX_UNDEF_P(o)  (reinterpret_cast<Object*>((o)) == RBX_Qundef)

#define RBX_RTEST(o)    ((reinterpret_cast<uintptr_t>(o) & 0xf) != 0xa)


/* Reset relative to our VALUEs */
#undef Qfalse
#undef Qtrue
#undef Qnil
#undef Qundef

#undef ALLOC
#undef ALLOC_N
#undef NIL_P
#undef RTEST



/** Global class/object etc. types. */
#ifdef __cplusplus
extern "C" {
#endif

  /**
   *  Global object abstraction.
   *
   *  @internal.
   */
  typedef enum {
    cCApiArray = 0,
    cCApiBignum,
    cCApiClass,
    cCApiComparable,
    cCApiData,
    cCApiEnumerable,
    cCApiFalse,
    cCApiFixnum,
    cCApiFloat,
    cCApiHash,
    cCApiInteger,
    cCApiIO,
    cCApiKernel,
    cCApiModule,
    cCApiNil,
    cCApiObject,
    cCApiRegexp,
    cCApiString,
    cCApiSymbol,
    cCApiThread,
    cCApiTrue,

    cCApiArgumentError,
    cCApiEOFError,
    cCApiErrno,
    cCApiException,
    cCApiFatal,
    cCApiFloatDomainError,
    cCApiIndexError,
    cCApiInterrupt,
    cCApiIOError,
    cCApiLoadError,
    cCApiLocalJumpError,
    cCApiNameError,
    cCApiNoMemoryError,
    cCApiNoMethodError,
    cCApiNotImplementedError,
    cCApiRangeError,
    cCApiRegexpError,
    cCApiRuntimeError,
    cCApiScriptError,
    cCApiSecurityError,
    cCApiSignalException,
    cCApiStandardError,
    cCApiSyntaxError,
    cCApiSystemCallError,
    cCApiSystemExit,
    cCApiSystemStackError,
    cCApiTypeError,
    cCApiThreadError,
    cCApiZeroDivisionError,

    // MUST be last
    cCApiMaxConstant
  } CApiConstant;


  /**
   *  Integral type map for MRI's types.
   *
   *  Rubinius does not implement all of these,
   *  so T_OBJECT is returned instead in those
   *  cases.
   */
  typedef enum {
    T_ARRAY,
    T_NONE,
    T_NIL,
    T_OBJECT,
    T_CLASS,
    T_ICLASS,
    T_MODULE,
    T_FLOAT,
    T_STRING,
    T_REGEXP,
    T_FIXNUM,
    T_HASH,
    T_STRUCT,
    T_BIGNUM,
    T_FILE,
    T_TRUE,
    T_FALSE,
    T_DATA,
    T_MATCH,
    T_SYMBOL,
    T_BLKTAG,
    T_UNDEF,
    T_VARMAP,
    T_SCOPE,
    T_NODE

  } CApiType;

  /**
   *  Method variants that can be defined.
   */
  typedef enum {
    cCApiPublicMethod,
    cCApiProtectedMethod,
    cCApiPrivateMethod,
    cCApiSingletonMethod

  } CApiMethodKind;

#ifdef __cplusplus
}
#endif

/*
 * The immediates.
 */

#define cCApiQfalse     (0)
#define cCApiQtrue      (-1)
#define cCApiQnil       (-2)
#define cCApiQundef     (-3)

/** The false object.
 *
 * NOTE: This is defined to be 0 because it is 0 in MRI and
 * extensions written for MRI have (absolutely wrongly,
 * infuriatingly, but-what-can-you-do-now) relied on that
 * assumption in boolean contexts. Rather than fighting a
 * myriad subtle bugs, we just go along with it.
 */
#define Qfalse ((VALUE)cCApiQfalse)
/** The true object. */
#define Qtrue  ((VALUE)cCApiQtrue)
/** The nil object. */
#define Qnil   ((VALUE)cCApiQnil)
/** The undef object. NEVER EXPOSE THIS TO USER CODE. EVER. */
#define Qundef ((VALUE)cCApiQundef)


/* Global Class objects */

#define rb_cArray             (capi_get_constant(cCApiArray))
#define rb_cBignum            (capi_get_constant(cCApiBignum))
#define rb_cClass             (capi_get_constant(cCApiClass))
#define rb_cData              (capi_get_constant(cCApiData))
#define rb_cFalseClass        (capi_get_constant(cCApiFalse))
#define rb_cFixnum            (capi_get_constant(cCApiFixnum))
#define rb_cFloat             (capi_get_constant(cCApiFloat))
#define rb_cHash              (capi_get_constant(cCApiHash))
#define rb_cInteger           (capi_get_constant(cCApiInteger))
#define rb_cIO                (capi_get_constant(cCApiIO))
#define rb_cModule            (capi_get_constant(cCApiModule))
#define rb_cNilClass          (capi_get_constant(cCApiNil))
#define rb_cObject            (capi_get_constant(cCApiObject))
#define rb_cRegexp            (capi_get_constant(cCApiRegexp))
#define rb_cString            (capi_get_constant(cCApiString))
#define rb_cSymbol            (capi_get_constant(cCApiSymbol))
#define rb_cThread            (capi_get_constant(cCApiThread))
#define rb_cTrueClass         (capi_get_constant(cCApiTrue))


/* Global Module objects. */

#define rb_mComparable        (capi_get_constant(cCApiComparable))
#define rb_mEnumerable        (capi_get_constant(cCApiEnumerable))
#define rb_mKernel            (capi_get_constant(cCApiKernel))


/* Exception classes. */

#define rb_eArgError          (capi_get_constant(cCApiArgumentError))
#define rb_eEOFError          (capi_get_constant(cCApiEOFError))
#define rb_mErrno             (capi_get_constant(cCApiErrno))
#define rb_eException         (capi_get_constant(cCApiException))
#define rb_eFatal             (capi_get_constant(cCApiFatal))
#define rb_eFloatDomainError  (capi_get_constant(cCApiFloatDomainError))
#define rb_eIndexError        (capi_get_constant(cCApiIndexError))
#define rb_eInterrupt         (capi_get_constant(cCApiInterrupt))
#define rb_eIOError           (capi_get_constant(cCApiIOError))
#define rb_eLoadError         (capi_get_constant(cCApiLoadError))
#define rb_eLocalJumpError    (capi_get_constant(cCApiLocalJumpError))
#define rb_eNameError         (capi_get_constant(cCApiNameError))
#define rb_eNoMemError        (capi_get_constant(cCApiNoMemoryError))
#define rb_eNoMethodError     (capi_get_constant(cCApiNoMethodError))
#define rb_eNotImpError       (capi_get_constant(cCApiNotImplementedError))
#define rb_eRangeError        (capi_get_constant(cCApiRangeError))
#define rb_eRegexpError       (capi_get_constant(cCApiRegexpError))
#define rb_eRuntimeError      (capi_get_constant(cCApiRuntimeError))
#define rb_eScriptError       (capi_get_constant(cCApiScriptError))
#define rb_eSecurityError     (capi_get_constant(cCApiSecurityError))
#define rb_eSignal            (capi_get_constant(cCApiSignalException))
#define rb_eStandardError     (capi_get_constant(cCApiStandardError))
#define rb_eSyntaxError       (capi_get_constant(cCApiSyntaxError))
#define rb_eSystemCallError   (capi_get_constant(cCApiSystemCallError))
#define rb_eSystemExit        (capi_get_constant(cCApiSystemExit))
#define rb_eSysStackError     (capi_get_constant(cCApiSystemStackError))
#define rb_eTypeError         (capi_get_constant(cCApiTypeError))
#define rb_eThreadError       (capi_get_constant(cCApiThreadError))
#define rb_eZeroDivError      (capi_get_constant(cCApiZeroDivisionError))


/* Interface macros */

/** Allocate memory for type. Must NOT be used to allocate Ruby objects. */
#define ALLOC(type)       (type*) malloc(sizeof(type))

/** Allocate memory for N of type. Must NOT be used to allocate Ruby objects. */
#define ALLOC_N(type, n)  (type*) malloc(sizeof(type) * (n))

/** Interrupt checking (no-op). */
#define CHECK_INTS        /* No-op */

/** Get the Class object is an instance of. */
#define CLASS_OF          rb_class_of

/** Convert a Fixnum into an int. */
#define FIX2INT(i)        NUM2INT((i))

/** Convert a Fixnum into a long. */
#define FIX2LONG(i)       NUM2LONG((i))

/** Convert a Fixnum into an unsigned int. */
#define FIX2UINT(i)       NUM2UINT((i))

/** Get a handle for the Symbol object represented by ID. */
#define ID2SYM(id)        capi_id2sym((id))

/** Infect o2 if o1 is tainted */
#define OBJ_INFECT(o1, o2) capi_infect((o1), (o2))

/** Convert int to a Ruby Integer. */
#define INT2FIX(i)        INT2NUM((i))

/** Convert long to a Ruby Integer. @todo Should we warn if overflowing? --rue */
#define LONG2FIX(i)       LONG2NUM((i))

/** Zero out N elements of type starting at given pointer. */
#define MEMZERO(p,type,n) memset((p), 0, (sizeof(type) * (n)))

/** Whether object is nil. */
#define NIL_P(v)          capi_nil_p((v))

/** The length of string str. */
#define RSTRING_LEN(str)  capi_rstring_len((str))

/** The pointer to the string str's data. */
#define RSTRING_PTR(str)  capi_rstring_ptr((str))

/** False if expression evaluates to nil or false, true otherwise. */
#define RTEST(v)          capi_rtest((v))

/** Return the super class of the object */
#define RCLASS_SUPER(klass)   capi_class_superclass((klass))

/** Rubinius' SafeStringValue is the same as StringValue. */
#define SafeStringValue   StringValue

/** Modifies the VALUE object in place by calling rb_obj_as_string(). */
#define StringValue(v)        rb_string_value(&(v))
#define StringValuePtr(v)     rb_string_value_ptr(&(v))
#define StringValueCStr(str)  rb_string_value_cstr(&(str))

/** Retrieve the ID given a Symbol handle. */
#define SYM2ID(sym)       capi_sym2id((sym))

/** Return an integer type id for the object. @see rb_type() */
#define TYPE(handle)      rb_type(handle)

/** Convert unsigned int to a Ruby Integer. @todo Should we warn if overflowing? --rue */
#define UINT2FIX(i)       UINT2NUM((i))



/**
 *
 *  @todo   The Handle management is ludicrously naive. --rue
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Secret extra stuff */

  typedef VALUE (*CApiAllocFunction)();
  typedef VALUE (*CApiGenericFunction)();


  /**
   *  \internal
   *
   *  Backend for defining methods after normalization.
   *
   *  @see  rb_define_*_method.
   */
  void    capi_define_method(const char* file,
                                           VALUE target,
                                           const char* name,
                                           CApiGenericFunction fptr,
                                           int arity,
                                           CApiMethodKind kind);

  /** Call method on receiver, args as varargs. */
  VALUE   capi_rb_funcall(const char* file, int line,
                                        VALUE receiver, ID method_name,
                                        int arg_count, ...);

  /** Call the method with args provided in a C array. */
  VALUE   capi_rb_funcall2(const char* file, int line,
                                         VALUE receiver, ID method_name,
                                         int arg_count, VALUE* args);

  /** Retrieve a Handle to a globally available object. @internal. */
  VALUE   capi_get_constant(CApiConstant type);

  /** Symbol Handle for an ID. @internal. */
  VALUE   capi_id2sym(ID id);

  /** Returns the string associated with a symbol. */
  const char *rb_id2name(ID sym);

  /** Infect obj2 if obj1 is tainted. @internal.*/
  void    capi_infect(VALUE obj1, VALUE obj2);

  /** False if expression evaluates to nil, true otherwise. @internal. */
  int     capi_nil_p(VALUE expression_result);

  /** Length of string string_handle. @internal. */
  long    capi_rstring_len(VALUE string_handle);

  /** Pointer to string data in string_handle. @internal. */
  char*   capi_rstring_ptr(VALUE string_handle);

  /** False if expression evaluates to Qnil or Qfalse, true otherwise. @internal. */
  int     capi_rtest(VALUE expression_result);

  /** ID from a Symbol Handle. @internal. */
  ID      capi_sym2id(VALUE symbol_handle);

  /** Return the data pointer in a Data object. */
  void**  capi_data_ptr_get_address(VALUE obj_handle);

  /** Returns the superclass of klass or NULL. This is not the same as
   * rb_class_superclass. See MRI's rb_class_s_alloc which returns a
   * class created with rb_class_boot(0), i.e. having a NULL superclass.
   * RCLASS_SUPER(klass) is used in a boolean context to exit a loop in
   * the Digest extension. It's likely other extensions do the same thing.
   */
  VALUE   capi_class_superclass(VALUE class_handle);


/* Real API */

  /** Convert an Integer into an int. */
  int     NUM2INT(VALUE num_handle);

  /** Convert an Integer into a long int. */
  long    NUM2LONG(VALUE num_handle);

  /** Convert an Integer into an unsigned int. */
  unsigned int NUM2UINT(VALUE num_handle);

  /** Convert an int into an Integer. */
  VALUE   INT2NUM(int number);

  /** Convert a long int into an Integer. */
  VALUE   LONG2NUM(long int number);

  /** Convert unsigned int into a Numeric. */
  VALUE   UINT2NUM(unsigned int number);

#define   Data_Make_Struct(klass, type, mark, free, sval) (\
            sval = ALLOC(type), \
            memset(sval, NULL, sizeof(type)), \
            Data_Wrap_Struct(klass, mark, free, sval)\
          )

#define   Data_Wrap_Struct(klass, mark, free, sval) \
            rb_data_object_alloc(klass, (void*)sval, (RUBY_DATA_FUNC)mark, \
                                 (RUBY_DATA_FUNC)free)

#define   DATA_PTR(obj_handle)   (*capi_data_ptr_get_address(obj_handle))

#define   Data_Get_Struct(obj,type,sval) do {\
            Check_Type(obj, T_DATA); \
            sval = (type*)DATA_PTR(obj);\
} while (0)

  /** Return obj if it is an Array, or return wrapped (i.e. [obj]) */
  VALUE   rb_Array(VALUE obj_handle);

  /** Remove all elements from the Array. Returns self. */
  VALUE   rb_ary_clear(VALUE self_handle);

  /** Return shallow copy of the Array. The elements are not dupped. */
  VALUE   rb_ary_dup(VALUE self_handle);

  /** Return object at index. Out-of-bounds access returns Qnil. */
  VALUE   rb_ary_entry(VALUE self_handle, int index);

  /** Array#join. Returns String with all elements to_s, with optional separator String. */
  VALUE   rb_ary_join(VALUE self_handle, VALUE separator_handle);

  /** New, empty Array. */
  VALUE   rb_ary_new();

  /** New Array of nil elements at given length. */
  VALUE   rb_ary_new2(unsigned long length);

  /** New Array of given length, filled with copies of given object. */
  VALUE   rb_ary_new4(unsigned long length, const VALUE* object_handle);

  /** Remove and return last element of Array or nil. */
  VALUE   rb_ary_pop(VALUE self_handle);

  /** Appends value to end of Array and returns self. */
  VALUE   rb_ary_push(VALUE self_handle, VALUE object_handle);

  /** Returns a new Array with elements in reverse order. Elements not dupped. */
  VALUE   rb_ary_reverse(VALUE self_handle);

  /** Remove and return first element of Array or nil. Changes other elements' indexes. */
  VALUE   rb_ary_shift(VALUE self_handle);

  /** Number of elements in given Array. @todo MRI specifies int return, problem? */
  size_t  rb_ary_size(VALUE self_handle);

  /** Store object at given index. Supports negative indexes. Returns object. */
  void    rb_ary_store(VALUE self_handle, long int index, VALUE object_handle);

  /** Add object to the front of Array. Changes old indexes +1. Returns object. */
  VALUE   rb_ary_unshift(VALUE self_handle, VALUE object_handle);

  /** @see rb_ivar_get */
  VALUE   rb_attr_get(VALUE obj_handle, ID attr_name);

  /** Return 1 if this send has a block, 0 otherwise. */
  int     rb_block_given_p();

  /** If object responds to #to_ary, returns the result of that call, otherwise nil. */
  VALUE   rb_check_array_type(VALUE object_handle);

  /** If object responds to #to_str, returns the result of that call, otherwise nil. */
  VALUE   rb_check_string_type(VALUE object_handle);

  /** Raises an exception if obj_handle is frozen. */
  void    rb_check_frozen(VALUE obj_handle);

  /** Raises an exception if obj_handle is not the same type as 'type'. */
  void    rb_check_type(VALUE obj_handle, CApiType type);

#define Check_Type(v,t) rb_check_type((VALUE)(v),t)

  /**
   *  Safe type conversion.
   *
   *  If the object responds to the given method name, the method is
   *  called and the result returned. Otherwise returns nil.
   *
   *  @see rb_check_array_type() and rb_check_string_type().
   */
  VALUE   rb_check_convert_type(VALUE object_handle, int type,
      const char* type_name, const char* method_name);

  /** Returns String representation of the class' name. */
  VALUE   rb_class_name(VALUE class_handle);

  /** As Ruby's .new, with the given arguments. Returns the new object. */
  VALUE   rb_class_new_instance(int arg_count, VALUE* args, VALUE class_handle);

  /** Returns the Class object this object is an instance of. */
  VALUE   rb_class_of(VALUE object_handle);

  /** C string representation of the class' name. You must free this string. */
  char*   rb_class2name(VALUE class_handle);

  /** Return the module referred to by qualified path (e.g. A::B::C) */
  VALUE   rb_path2class(const char*);

  /** Returns object returned by invoking method on object if right type, or raises error. */
  VALUE   rb_convert_type(VALUE object_handle, int type,
      const char* type_name, const char* method_name);

  /** Nonzero if constant corresponding to Symbol exists in the Module. */
  int     rb_const_defined(VALUE module_handle, ID const_id);

  /** Retrieve constant from given module. */
  VALUE   rb_const_get(VALUE module_handle, ID name);

  /** Return Integer obtained from String#to_i using given base. */
  VALUE   rb_cstr2inum(const char* string, int base);

  /** Returns module's named class variable. @@ is optional. */
  VALUE   rb_cv_get(VALUE module_handle, const char* name);

  /** Set module's named class variable to given value. Returns the value. @@ is optional. */
  VALUE   rb_cv_set(VALUE module_handle, const char* name, VALUE value);

  /** Returns a value evaluating true if module has named class var. @@ is optional. */
  VALUE   rb_cvar_defined(VALUE module_handle, ID name);

  /** Returns class variable by (Symbol) name from module. @@ is optional. */
  VALUE   rb_cvar_get(VALUE module_handle, ID name);

  /** Set module's named class variable to given value. Returns the value. @@ is optional. */
  VALUE   rb_cvar_set(VALUE module_handle, ID name, VALUE value, int unused);

  VALUE   rb_data_object_alloc(VALUE klass, void* sval,
      RUBY_DATA_FUNC mark, RUBY_DATA_FUNC free);

  /** Alias method by old name as new name. Methods are independent of eachother. */
  void    rb_define_alias(VALUE module_handle, const char *new_name, const char *old_name);

  /** Define an .allocate for the given class. Should take no args and return a VALUE. */
  void    rb_define_alloc_func(VALUE class_handle, CApiAllocFunction allocator);

  /** Ruby's attr_* for given name. Nonzeros to toggle read/write. */
  void    rb_define_attr(VALUE module_handle, const char* attr_name,
      int readable, int writable);

  /** Reopen or create new top-level class with given superclass and name. Returns the Class object. */
  VALUE   rb_define_class(const char* name, VALUE superclass_handle);

  /** Reopen or create new class with superclass and name under parent module. Returns the Class object. */
  VALUE   rb_define_class_under(VALUE parent_handle, const char* name, VALUE superclass_handle);

  /** Define a constant in given Module's namespace. */
  void    rb_define_const(VALUE module_handle, const char* name, VALUE obj_handle);

  /** Generate a NativeMethod to represent a method defined as a C function. Records file. */
  #define rb_define_method(mod, name, fptr, arity) \
          capi_define_method(__FILE__, mod, name, \
                                           (CApiGenericFunction)fptr, arity, \
                                           cCApiPublicMethod)

  /** Reopen or create new top-level Module. */
  VALUE   rb_define_module(const char* name);

  /** Defines the given method as a private instance method and a singleton method of module. */
  void    rb_define_module_function(VALUE module_handle, const char* name, CApiGenericFunction func, int args);

  /** Reopen or create a new Module inside given parent Module. */
  VALUE   rb_define_module_under(VALUE parent_handle, const char* name);

  /** Generate a NativeMethod to represent a private method defined in the C function. */
  #define rb_define_private_method(mod, name, fptr, arity) \
          capi_define_method(__FILE__, mod, name, \
                                           (CApiGenericFunction)fptr, arity, \
                                           cCApiPrivateMethod)

  /** Generate a NativeMethod to represent a protected method defined in the C function. */
  #define rb_define_protected_method(mod, name, fptr, arity) \
          capi_define_method(__FILE__, mod, name, \
                                           (CApiGenericFunction)fptr, arity, \
                                           cCApiProtectedMethod)

  /** Generate a NativeMethod to represent a singleton method. @see capi_define_method. */
  #define rb_define_singleton_method(mod, name, fptr, arity) \
          capi_define_method(__FILE__, mod, name, \
                                           (CApiGenericFunction)fptr, arity, \
                                           cCApiSingletonMethod)

  /** Remove a previously declared global variable. */
  void    rb_free_global(VALUE global_handle);

  /**
   *  Freeze object and return it.
   *
   *  NOT supported in Rubinius. Just returns itself.
   */
  #define rb_obj_freeze(object_handle) \
          (object_handle)

  /**
   *  Call method on receiver, args as varargs. Calls private methods.
   *
   *  @todo Requires C99, change later for production code if needed.
   *        Pretty much all C++ compilers support this too.  It can be
   *        done by introducing an intermediary function to grab the
   *        debug info, but it is far uglier. --rue
   *
   *  See http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
   *  regarding use of ##__VA_ARGS__.
   */
  #define rb_funcall(receiver, method_name, arg_count, ...) \
          capi_rb_funcall(__FILE__, __LINE__, \
                                        (receiver), (method_name), \
                                        (arg_count) , ##__VA_ARGS__)

  /** Call the method with args provided in a C array. Calls private methods. */
  #define rb_funcall2(receiver, method_name, arg_count, args) \
          capi_rb_funcall2(__FILE__, __LINE__, \
                                         (receiver), (method_name), \
                                         (arg_count), (args) )

  /** @todo define rb_funcall3, which is the same as rb_funcall2 but
   * will not call private methods.
   */

  /** Mark ruby object ptr. */
  void    rb_gc_mark(VALUE ptr);

  /** Mark variable global. Will not be GC'd. */
  void    rb_global_variable(VALUE* handle_address);

  /** Retrieve global by name. Because of MRI, the leading $ is optional but recommended. */
  VALUE   rb_gv_get(const char* name);

  /** Set named global to given value. Returns value. $ optional. */
  VALUE   rb_gv_set(const char* name, VALUE value);

  /** Include Module in another Module, just as Ruby's Module#include. */
  void    rb_include_module(VALUE includer_handle, VALUE includee_handle);

  /** Convert string to an ID */
  ID      rb_intern(const char* string);

  /** Call #initialize on the object with given arguments. */
  void    rb_obj_call_init(VALUE object_handle, int arg_count, VALUE* args);

  /** Returns the Class object this object is an instance of. */
  #define rb_obj_class(object_handle) \
          rb_class_of((object_handle))

  /** String representation of the object's class' name. You must free this string. */
  char*   rb_obj_classname(VALUE object_handle);

  /** Returns true-ish if object is an instance of specific class. */
  VALUE   rb_obj_is_instance_of(VALUE object_handle, VALUE class_handle);

  /** Returns true-ish if module is object's class or other ancestor. */
  VALUE   rb_obj_is_kind_of(VALUE object_handle, VALUE module_handle);

  /** Return object's instance variable by name. @ optional. */
  VALUE   rb_iv_get(VALUE self_handle, const char* name);

  /** Set instance variable by name to given value. Returns the value. @ optional. */
  VALUE   rb_iv_set(VALUE self_handle, const char* name, VALUE value);

  /** Get object's instance variable. */
  VALUE   rb_ivar_get(VALUE obj_handle, ID ivar_name);

  /** Set object's instance variable to given value. */
  VALUE   rb_ivar_set(VALUE obj_handle, ID ivar_name, VALUE value);

  /** Checks if obj_handle has an ivar named ivar_name. */
  VALUE   rb_ivar_defined(VALUE obj_handle, ID ivar_name);

  /** Allocate uninitialised instance of given class. */
  VALUE   rb_obj_alloc(VALUE klass);

  /** Call #to_s on object. */
  VALUE   rb_obj_as_string(VALUE obj_handle);

  /** Return a clone of the object by calling the method bound
   * to Kernel#clone (i.e. does NOT call specialized #clone method
   * on obj_handle if one exists).
   */
  VALUE rb_obj_clone(VALUE obj_handle);

  /** Call #inspect on an object. */
  VALUE rb_inspect(VALUE obj_handle);

  /**
   *  Raise error of given class using formatted message.
   *
   *  @todo Implement for real. --rue
   */
  void    rb_raise(VALUE error_handle, const char* format_string, ...);

  /**
   *  Require a Ruby file.
   *
   *  Returns true on first load, false if already loaded or raises.
   */
  VALUE   rb_require(const char* name);

  /** 1 if obj.respond_to? method_name evaluates true, 0 otherwise. */
  int     rb_respond_to(VALUE obj_handle, ID method_name);

  /** Returns the current $SAFE level. */
  int     rb_safe_level();

  /**
   *  Process arguments using a template rather than manually.
   *
   *  The first two arguments are simple: the number of arguments given
   *  and an array of the args. Usually you get these as parameters to
   *  your function.
   *
   *  The spec works like this: it must have one (or more) of the following
   *  specifiers, and the specifiers that are given must always appear
   *  in the order given here. If the first character is a digit (0-9),
   *  it is the number of required parameters. If there is a second digit
   *  (0-9), it is the number of optional parameters. The next character
   *  may be "*", indicating a "splat" i.e. it consumes all remaining
   *  parameters. Finally, the last character may be "&", signifying
   *  that the block given (or Qnil) should be stored.
   *
   *  The remaining arguments are pointers to the variables in which
   *  the aforementioned format assigns the scanned parameters. For
   *  example in some imaginary function:
   *
   *    VALUE required1, required2, optional, splat, block
   *    rb_scan_args(argc, argv, "21*&", &required1, &required2,
   *                                     &optional,
   *                                     &splat,
   *                                     &block);
   *
   *  The required parameters must naturally always be exact. The
   *  optional parameters are set to nil when parameters run out.
   *  The splat is always an Array, but may be empty if there were
   *  no parameters that were not consumed by required or optional.
   *  Lastly, the block may be nil.
   */
  int     rb_scan_args(int argc, const VALUE* argv, const char* spec, ...);

  /** Raise error if $SAFE is not higher than the given level. */
  void    rb_secure(int level);

  /** Set $SAFE to given _higher_ level. Lowering $SAFE is not allowed. */
  void    rb_set_safe_level(int new_level);

  /** Returns the MetaClass object of the object. */
  VALUE   rb_singleton_class(VALUE object_handle);

  /** Tries to return a String using #to_str. Error raised if no or invalid conversion. */
  VALUE   rb_String(VALUE object_handle);

  /** Appends other String to self and returns the modified self. */
  VALUE   rb_str_append(VALUE self_handle, VALUE other_handle);

  /** Appends other String to self and returns self. @see rb_str_append */
  VALUE   rb_str_buf_append(VALUE self_handle, VALUE other_handle);

  /** Append given number of bytes from C string to and return the String. */
  VALUE   rb_str_buf_cat(VALUE string_handle, const char* other, size_t size);

  /** Append C string to and return the String. Uses strlen(). @see rb_str_buf_cat */
  VALUE   rb_str_buf_cat2(VALUE string_handle, const char* other);

  /**
   *  Return new empty String with preallocated storage.
   *
   *  @note   Not supported by Rubinius for a few more days.
   *  @todo   Update as soon as
   */
  VALUE   rb_str_buf_new(long capacity);

  /** Return new String concatenated of the two. */
  VALUE   rb_str_cat(VALUE string_handle, const char* other, size_t length);

  /** Return new String concatenated of the two. Uses strlen(). @see rb_str_cat */
  VALUE   rb_str_cat2(VALUE string_handle, const char* other);

  /** Compare Strings as Ruby String#<=>. Returns -1, 0 or 1. */
  int     rb_str_cmp(VALUE first_handle, VALUE second_handle);

  /** Append other String or character to self, and return the modified self. */
  VALUE   rb_str_concat(VALUE self_handle, VALUE other_handle);

  /** As Ruby's String#dup, returns copy of self as a new String. */
  VALUE   rb_str_dup(VALUE self_handle);

  /** Replace contents of string with the C string. */
  void    rb_str_flush_char_ptr(VALUE string_handle, char* c_string, size_t length);

  /** Return character at offset in String. Out-of-bounds returns '\0'. */
  char    rb_str_get_char(VALUE self_handle, int offset);

  /** Return number of bytes in String. @todo MRI returns int, problem? */
  size_t  rb_str_get_char_len(VALUE self_handle);

  /**
   *  Returns a _copy_ of the C string contained in given String.
   *
   *  You must free the string.
   */
  char*   rb_str_get_char_ptr(VALUE str_handle);

  /** Create a String using the designated length of given C string. */
  VALUE   rb_str_new(const char* string, size_t length);

  /** Create a String from a C string. */
  VALUE   rb_str_new2(const char* string);

  /** Returns a new String created from concatenating self with other. */
  VALUE   rb_str_plus(VALUE self_handle, VALUE other_handle);

  /** Splits self using the separator string. Returns Array of substrings. */
  VALUE   rb_str_split(VALUE self_handle, const char* separator);

  /**
   *  As Ruby's String#slice.
   *
   *  Returns new String with copy of length characters
   *  starting from the given index of self. The index
   *  may be negative. Normal String#slice border conditions
   *  apply.
   */
  VALUE   rb_str_substr(VALUE self_handle, size_t starting_index, size_t length);

  /** Return a new String containing given number of copies of self. */
  VALUE   rb_str_times(VALUE self_handle, VALUE times);

  /** Return an Integer obtained from String#to_i, using the given base. */
  VALUE   rb_str2inum(VALUE self_handle, int base);

  /** Try to return a String using #to_str. Error raised if no or invalid conversion. */
  VALUE   rb_str_to_str(VALUE object_handle);

  /** Call #to_s on object pointed to and _replace_ it with the String. */
  VALUE   rb_string_value(VALUE* object_variable);

  char*   rb_string_value_ptr(VALUE* object_variable);
  /**
   *  As rb_string_value but also returns a C string of the new String.
   *
   *  You must free the string.
   */
  char*   rb_string_value_cstr(VALUE* object_variable);

  /** Create a String from the C string. Rubinius does not implement tainting. */
  VALUE   rb_tainted_str_new2(const char* string);

  /** Issue a thread.pass. */
  void    rb_thread_schedule();

  /** Returns an integer value representing the object's type. */
  int     rb_type(VALUE object_handle);

  /** Call #to_sym on object. */
  ID      rb_to_id(VALUE object_handle);

  /** Module#undefine_method. Objects of class will not respond to name. @see rb_remove_method */
  void    rb_undef_method(VALUE module_handle, const char* name);

  /** Call block with given argument or raise error if no block given. */
  VALUE   rb_yield(VALUE argument_handle);

#ifdef __cplusplus
}
#endif

#endif
