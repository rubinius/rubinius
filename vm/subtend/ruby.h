#ifndef RBX_SUBTEND_RUBY_H
#define RBX_SUBTEND_RUBY_H

/**
 *  @file
 *
 *  Notes:
 *
 *    - The function prefix rbx_subtend_hidden_* is used for those
 *      functions that must be available for user code but should
 *      not be used directly.
 *
 *    - The prefix hidden_* is used inside ruby.cpp only, and each
 *      of the functions is defined there as static in an anonymous
 *      namespace. These functions do not need to be usable by user
 *      code.
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
 *  @todo Move the context to a hidden last parameter defaulting to
 *        current() so that we can pass around the context whenever
 *        possible? --rue
 */

#include <stddef.h>
#include <stdint.h>

#define RUBY

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
#define RBX_Qfalse      (reinterpret_cast<Object*>(6UL))
#define RBX_Qtrue       (reinterpret_cast<Object*>(10UL))
#define RBX_Qnil        (reinterpret_cast<Object*>(14UL))
#define RBX_Qundef      (reinterpret_cast<Object*>(18UL))

#define RBX_FALSE_P(o)  (reinterpret_cast<Object*>((o)) == RBX_Qfalse)
#define RBX_TRUE_P(o)   (reinterpret_cast<Object*>((o)) == RBX_Qtrue)
#define RBX_NIL_P(o)    (reinterpret_cast<Object*>((o)) == RBX_Qnil)
#define RBX_UNDEF_P(o)  (reinterpret_cast<Object*>((o)) == RBX_Qundef)

#define RBX_RTEST(o)    ((reinterpret_cast<uintptr_t>(o) & 0x7) != 0x6)


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
    RbxArray,
    RbxBignum,
    RbxClass,
    RbxData,
    RbxFalse,
    RbxFixnum,
    RbxFloat,
    RbxHash,
    RbxInteger,
    RbxIO,
    RbxModule,
    RbxNil,
    RbxObject,
    RbxRegexp,
    RbxString,
    RbxSymbol,
    RbxThread,
    RbxTrue,

    RbxComparable,
    RbxEnumerable,
    RbxKernel

  } RbxSubtendHiddenGlobal;

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

  } RbxSubtendMRIType;

  /**
   *  Error class abstraction.
   *
   *  @internal.
   */
  typedef enum {
    RbxArgumentError,
    RbxEOFError,
    RbxErrno,
    RbxException,
    RbxFatal,
    RbxFloatDomainError,
    RbxIndexError,
    RbxInterrupt,
    RbxIOError,
    RbxLoadError,
    RbxLocalJumpError,
    RbxNameError,
    RbxNoMemoryError,
    RbxNoMethodError,
    RbxNotImplementedError,
    RbxRangeError,
    RbxRegexpError,
    RbxRuntimeError,
    RbxScriptError,
    RbxSecurityError,
    RbxSignalException,
    RbxStandardError,
    RbxSyntaxError,
    RbxSystemCallError,
    RbxSystemExit,
    RbxSystemStackError,
    RbxTypeError,
    RbxThreadError,
    RbxZeroDivisionError

  } RbxSubtendHiddenError;

  /**
   *  Method variants that can be defined.
   */
  typedef enum {
    RbxPublicMethod,
    RbxProtectedMethod,
    RbxPrivateMethod,
    RbxSingletonMethod

  } RbxMethodKind;

#ifdef __cplusplus
}
#endif

/* The immediates.
 *
 * Hardcoded in NativeMethodContext::register_class_with(),
 * so change both files if absolutely necessary.
 */

/** The false object. */
#define Qfalse ((VALUE)-1)
/** The true object. */
#define Qtrue  ((VALUE)-2)
/** The nil object. */
#define Qnil   ((VALUE)-3)
/** The undef object. NEVER EXPOSE THIS TO USER CODE. EVER. */
#define Qundef ((VALUE)-4)


/* Global Class objects */

#define rb_cArray             (rbx_subtend_hidden_global(RbxArray))
#define rb_cBignum            (rbx_subtend_hidden_global(RbxBignum))
#define rb_cClass             (rbx_subtend_hidden_global(RbxClass))
#define rb_cData              (rbx_subtend_hidden_global(RbxData))
#define rb_cFalseClass        (rbx_subtend_hidden_global(RbxFalse))
#define rb_cFixnum            (rbx_subtend_hidden_global(RbxFixnum))
#define rb_cFloat             (rbx_subtend_hidden_global(RbxFloat))
#define rb_cHash              (rb_const_get(rb_cObject, rb_intern("Hash")))
#define rb_cInteger           (rbx_subtend_hidden_global(RbxInteger))
#define rb_cIO                (rbx_subtend_hidden_global(RbxIO))
#define rb_cModule            (rbx_subtend_hidden_global(RbxModule))
#define rb_cNilClass          (rbx_subtend_hidden_global(RbxNil))
#define rb_cObject            (rbx_subtend_hidden_global(RbxObject))
#define rb_cRegexp            (rbx_subtend_hidden_global(RbxRegexp))
#define rb_cString            (rbx_subtend_hidden_global(RbxString))
#define rb_cSymbol            (rbx_subtend_hidden_global(RbxSymbol))
#define rb_cThread            (rbx_subtend_hidden_global(RbxThread))
#define rb_cTrueClass         (rbx_subtend_hidden_global(RbxTrue))


/* Global Module objects. */

#define rb_mComparable        (rb_const_get(rb_cObject, rb_intern("Comparable")))
#define rb_mEnumerable        (rb_const_get(rb_cObject, rb_intern("Enumerable")))
#define rb_mKernel            (rb_const_get(rb_cObject, rb_intern("Kernel")))


/* Exception classes. */

#define rb_eArgError          (rbx_subtend_hidden_error(RbxArgumentError))
#define rb_eEOFError          (rbx_subtend_hidden_error(RbxEOFError))
#define rb_mErrno             (rbx_subtend_hidden_error(RbxErrno))    /* NOTE: This is a module. */
#define rb_eException         (rbx_subtend_hidden_error(RbxException))
#define rb_eFatal             (rbx_subtend_hidden_error(RbxFatal))
#define rb_eFloatDomainError  (rbx_subtend_hidden_error(RbxFloatDomainError))
#define rb_eIndexError        (rbx_subtend_hidden_error(RbxIndexError))
#define rb_eInterrupt         (rbx_subtend_hidden_error(RbxInterrupt))
#define rb_eIOError           (rbx_subtend_hidden_error(RbxIOError))
#define rb_eLoadError         (rbx_subtend_hidden_error(RbxLoadError))
#define rb_eLocalJumpError    (rbx_subtend_hidden_error(RbxLocalJumpError))
#define rb_eNameError         (rbx_subtend_hidden_error(RbxNameError))
#define rb_eNoMemError        (rbx_subtend_hidden_error(RbxNoMemoryError))
#define rb_eNoMethodError     (rbx_subtend_hidden_error(RbxNoMethodError))
#define rb_eNotImpError       (rbx_subtend_hidden_error(RbxNotImplementedError))
#define rb_eRangeError        (rbx_subtend_hidden_error(RbxRangeError))
#define rb_eRegexpError       (rbx_subtend_hidden_error(RbxRegexpError))
#define rb_eRuntimeError      (rbx_subtend_hidden_error(RbxRuntimeError))
#define rb_eScriptError       (rbx_subtend_hidden_error(RbxScriptError))
#define rb_eSecurityError     (rbx_subtend_hidden_error(RbxSecurityError))
#define rb_eSignal            (rbx_subtend_hidden_error(RbxSignalException))
#define rb_eStandardError     (rbx_subtend_hidden_error(RbxStandardError))
#define rb_eSyntaxError       (rbx_subtend_hidden_error(RbxSyntaxError))
#define rb_eSystemCallError   (rbx_subtend_hidden_error(RbxSystemCallError))
#define rb_eSystemExit        (rbx_subtend_hidden_error(RbxSystemExit))
#define rb_eSysStackError     (rbx_subtend_hidden_error(RbxSystemStackError))
#define rb_eTypeError         (rbx_subtend_hidden_error(RbxTypeError))
#define rb_eThreadError       (rbx_subtend_hidden_error(RbxThreadError))
#define rb_eZeroDivError      (rbx_subtend_hidden_error(RbxZeroDivisionError))


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
#define ID2SYM(id)        rbx_subtend_hidden_id2sym((id))

/** Infect o2 if o1 is tainted */
#define OBJ_INFECT(o1, o2) rbx_subtend_hidden_infect((o1), (o2))

/** Convert int to a Ruby Integer. */
#define INT2FIX(i)        INT2NUM((i))

/** Convert long to a Ruby Integer. @todo Should we warn if overflowing? --rue */
#define LONG2FIX(i)       LONG2NUM((i))

/** Zero out N elements of type starting at given pointer. */
#define MEMZERO(p,type,n) memset((p), 0, (sizeof(type) * (n)))

/** Whether object is nil. */
#define NIL_P(v)          rbx_subtend_hidden_nil_p((v))

/** The length of string str. */
#define RSTRING_LEN(str)  rbx_subtend_hidden_rstring_len((str))

/** The pointer to the string str's data. */
#define RSTRING_PTR(str)  rbx_subtend_hidden_rstring_ptr((str))

/** False if expression evaluates to nil or false, true otherwise. */
#define RTEST(v)          rbx_subtend_hidden_rtest((v))

/** Rubinius' SafeStringValue is the same as StringValue. */
#define SafeStringValue   StringValue

/** Modifies the VALUE object in place by calling rb_obj_as_string(). */
#define StringValue(v)    rb_string_value(&(v))

#define StringValueCStr(str) rb_string_value_cstr(&(str))

/** Retrieve the ID given a Symbol handle. */
#define SYM2ID(sym)       rbx_subtend_hidden_sym2id((sym))

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

  typedef VALUE (*SubtendAllocFunction)(void);
  typedef void (*SubtendGenericFunction)(void);


  /**
   *  \internal
   *
   *  Backend for defining methods after normalization.
   *
   *  @see  rb_define_*_method.
   */
  void    rbx_subtend_hidden_define_method(const char* file,
                                           VALUE target,
                                           const char* name,
                                           SubtendGenericFunction fptr,
                                           int arity,
                                           RbxMethodKind kind);

  /** Retrieve a Handle to an error class. @internal. */
  VALUE   rbx_subtend_hidden_error(RbxSubtendHiddenError type);

  /** Call method on receiver, args as varargs. */
  VALUE   rbx_subtend_hidden_rb_funcall(const char* file, int line,
                                        VALUE receiver, ID method_name,
                                        int arg_count, ...);

  /** Call the method with args provided in a C array. */
  VALUE   rbx_subtend_hidden_rb_funcall2(const char* file, int line,
                                         VALUE receiver, ID method_name,
                                         int arg_count, VALUE* args);

  /** Retrieve a Handle to a globally available object. @internal. */
  VALUE   rbx_subtend_hidden_global(RbxSubtendHiddenGlobal type);

  /** Symbol Handle for an ID. @internal. */
  VALUE   rbx_subtend_hidden_id2sym(ID id);

  /** Infect obj2 if obj1 is tainted. @internal.*/
  void    rbx_subtend_hidden_infect(VALUE obj1, VALUE obj2);

  /** False if expression evaluates to nil, true otherwise. @internal. */
  int     rbx_subtend_hidden_nil_p(VALUE expression_result);

  /** Length of string string_handle. @internal. */
  long    rbx_subtend_hidden_rstring_len(VALUE string_handle);

  /** Pointer to string data in string_handle. @internal. */
  char*   rbx_subtend_hidden_rstring_ptr(VALUE string_handle);

  /** False if expression evaluates to Qnil or Qfalse, true otherwise. @internal. */
  int     rbx_subtend_hidden_rtest(VALUE expression_result);

  /** ID from a Symbol Handle. @internal. */
  ID      rbx_subtend_hidden_sym2id(VALUE symbol_handle);



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

  char*   StringValuePtr(VALUE str);

#define   Data_Make_Struct(klass, type, mark, free, sval) (\
            sval = ALLOC(type), \
            memset(sval, NULL, sizeof(type)), \
            Data_Wrap_Struct(klass, mark, free, sval)\
          )

#define   Data_Wrap_Struct(klass, mark, free, sval) \
            rb_data_object_alloc(klass, (void*)sval, (RUBY_DATA_FUNC)mark, \
                                 (RUBY_DATA_FUNC)free)

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

  /**
   *  Safe type conversion.
   *
   *  If the object responds to the given method name, the method is
   *  called and the result returned. Otherwise returns nil.
   *
   *  @see rb_check_array_type() and rb_check_string_type().
   */
  VALUE   rb_check_convert_type(VALUE object_handle, int type, const char* type_name, const char* method_name);

  /** Returns String representation of the class' name. */
  VALUE   rb_class_name(VALUE class_handle);

  /** As Ruby's .new, with the given arguments. Returns the new object. */
  VALUE   rb_class_new_instance(int arg_count, VALUE* args, VALUE class_handle);

  /** Returns the Class object this object is an instance of. */
  VALUE   rb_class_of(VALUE object_handle);

  /** C string representation of the class' name. You must free this string. */
  char*   rb_class2name(VALUE class_handle);

  /** Returns object returned by invoking method on object if right type, or raises error. */
  VALUE   rb_convert_type(VALUE object_handle, int type, const char* type_name, const char* method_name);

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
  VALUE   rb_cvar_set(VALUE module_handle, ID name, VALUE value);

  VALUE   rb_data_object_alloc(VALUE klass, RUBY_DATA_FUNC mark,
                               RUBY_DATA_FUNC free, void* sval);
  /** Alias method by old name as new name. Methods are independent of eachother. */
  void    rb_define_alias(VALUE module_handle, const char *new_name, const char *old_name);

  /** Define an .allocate for the given class. Should take no args and return a VALUE. */
  void    rb_define_alloc_func(VALUE class_handle, SubtendAllocFunction allocator);

  /** Ruby's attr_* for given name. Nonzeros to toggle read/write. */
  void    rb_define_attr(VALUE module_handle, const char* attr_name, int readable, int writable);

  /** Reopen or create new top-level class with given superclass and name. Returns the Class object. */
  VALUE   rb_define_class(const char* name, VALUE superclass_handle);

  /** Reopen or create new class with superclass and name under parent module. Returns the Class object. */
  VALUE   rb_define_class_under(VALUE parent_handle, const char* name, VALUE superclass_handle);

  /** Define a constant in given Module's namespace. */
  void    rb_define_const(VALUE module_handle, const char* name, VALUE obj_handle);

  /** Generate a NativeMethod to represent a method defined as a C function. Records file. */
  #define rb_define_method(mod, name, fptr, arity) \
          rbx_subtend_hidden_define_method(__FILE__, mod, name, \
                                           (SubtendGenericFunction)fptr, arity, \
                                           RbxPublicMethod)

  /** Reopen or create new top-level Module. */
  VALUE   rb_define_module(const char* name);

  /** Defines the given method as a private instance method and a singleton method of module. */
  void    rb_define_module_function(VALUE module_handle, const char* name, SubtendGenericFunction func, int args);

  /** Reopen or create a new Module inside given parent Module. */
  VALUE   rb_define_module_under(VALUE parent_handle, const char* name);

  /** Generate a NativeMethod to represent a private method defined in the C function. */
  #define rb_define_private_method(mod, name, fptr, arity) \
          rbx_subtend_hidden_define_method(__FILE__, mod, name, \
                                           (SubtendGenericFunction)fptr, arity, \
                                           RbxPrivateMethod)

  /** Generate a NativeMethod to represent a protected method defined in the C function. */
  #define rb_define_protected_method(mod, name, fptr, arity) \
          rbx_subtend_hidden_define_method(__FILE__, mod, name, \
                                           (SubtendGenericFunction)fptr, arity, \
                                           RbxProtectedMethod)

  /** Generate a NativeMethod to represent a singleton method. @see rbx_subtend_hidden_define_method. */
  #define rb_define_singleton_method(mod, name, fptr, arity) \
          rbx_subtend_hidden_define_method(__FILE__, mod, name, \
                                           (SubtendGenericFunction)fptr, arity, \
                                           RbxSingletonMethod)

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
   *  Call method on receiver, args as varargs.
   *
   *  @todo Requires C99, change later for production code if needed.
   *        Pretty much all C++ compilers support this too.  It can be
   *        done by introducing an intermediary function to grab the
   *        debug info, but it is far uglier. --rue
   */
  #define rb_funcall(receiver, method_name, arg_count, ...) \
          rbx_subtend_hidden_rb_funcall(__FILE__, __LINE__, \
                                        (receiver), (method_name), \
                                        (arg_count), __VA_ARGS__)

  /** Call the method with args provided in a C array. */
  #define rb_funcall2(receiver, method_name, arg_count, args) \
          rbx_subtend_hidden_rb_funcall2(__FILE__, __LINE__, \
                                         (receiver), (method_name), \
                                         (arg_count), (args) )

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

  /** Allocate and return new, uninitialised object of given class. */
  VALUE   rb_obj_alloc(VALUE class_handle);

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

  /** Allocate uninitialised instance of given class. */
  VALUE   rb_obj_alloc(VALUE klass);

  /** Call #to_s on object. */
  VALUE   rb_obj_as_string(VALUE obj_handle);

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

  /** Frees x */
  void    ruby_xfree(void* x);


#ifdef __cplusplus
}
#endif

#endif

