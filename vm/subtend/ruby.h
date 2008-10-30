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

/** Allocate memory for type. Must not be used to allocate Ruby objects. */
#define ALLOC(type)       (type*) malloc(sizeof(type))

/** Allocate memory for N of type. Must not be used to allocate Ruby objects. */
#define ALLOC_N(type, n)  (type*) malloc(sizeof(type) * (n))

/** Interrupt checking (no-op). */
#define CHECK_INTS        /* No-op */

/** Get a handle for the Symbol object represented by ID. */
#define ID2SYM(id)        rbx_subtend_hidden_id2sym((id))

/** Zero out N elements of type starting at given pointer. */
#define MEMZERO(p,type,n) memset((p), 0, (sizeof(type) * (n)))

/** Whether object is nil. */
#define NIL_P(v)          rbx_subtend_hidden_nil_p((v))

/** False if expression evaluates to nil or false, true otherwise. */
#define RTEST(v)          rbx_subtend_hidden_rtest((v))

/** Convert numeric into an int. */
#define NUM2INT(n)        FIX2INT((n))

/** Rubinius' SafeStringValue is the same as StringValue. */
#define SafeStringValue   StringValue

/** Modifies the VALUE object in place by calling rb_obj_as_string(). */
#define StringValue(v)    rb_string_value(&(v))

/** Retrieve the ID given a Symbol handle. */
#define SYM2ID(sym)       rbx_subtend_hidden_sym2id((sym))


/**
 *
 *  @todo   The Handle management is ludicrously naive. --rue
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Secret extra stuff */

  typedef void (*SubtendGenericFunction)(void);

  /** Backend for defining methods after normalization. @see rb_define_*_method. @internal. */
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

  /** False if expression evaluates to nil, true otherwise. @internal. */
  int     rbx_subtend_hidden_nil_p(VALUE expression_result);

  /** False if expression evaluates to Qnil or Qfalse, true otherwise. @internal. */
  int     rbx_subtend_hidden_rtest(VALUE expression_result);

  /** ID from a Symbol Handle. @internal. */
  ID      rbx_subtend_hidden_sym2id(VALUE symbol_handle);



/* Real API */


  /** Convert a Fixnum (well, Integer) into an int. */
  int     FIX2INT(VALUE num_handle);

  /** Convert an int into an Integer. */
  VALUE   INT2NUM(int number);

  /** Return obj if it is an Array, or return wrapped (i.e. [obj]) */
  VALUE   rb_Array(VALUE obj_handle);

  /** Return object at index. Out-of-bounds access returns Qnil. */
  VALUE   rb_ary_entry(VALUE self_handle, int index);

  /** Number of elements in given Array. @todo MRI specifies int return, problem? */
  size_t  rb_ary_size(VALUE self_handle);

  /** @see rb_ivar_get */
  VALUE   rb_attr_get(VALUE obj_handle, ID attr_name);

  /** Return 1 if this send has a block, 0 otherwise. */
  int     rb_block_given_p();

  /** Retrieve constant from given module. */
  VALUE   rb_const_get(VALUE module_handle, ID name);

  /** Define a constant in given Module's namespace. */
  void    rb_define_const(VALUE module_handle, const char* name, VALUE obj_handle);

  /** Generate a NativeMethod to represent a method defined as a C function. Records file. */
  #define rb_define_method(mod, name, fptr, arity) \
          rbx_subtend_hidden_define_method(__FILE__, mod, name, (SubtendGenericFunction)fptr, arity, 0)

  /** Create a new top-level Module. */
  VALUE   rb_define_module(const char* name);

  /** Create a new Module inside given parent Module. */
  VALUE   rb_define_module_under(VALUE parent_handle, const char* name);

  /** Generate a NativeMethod to represent a singleton method. @see rbx_subtend_hidden_define_method. */
  #define rb_define_singleton_method(mod, name, fptr, arity) \
          rbx_subtend_hidden_define_method(__FILE__, mod, name, (SubtendGenericFunction)fptr, arity, RbxSingletonMethod)

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

  /** Mark variable global. Will not be GC'd. */
  void    rb_global_variable(VALUE* handle_address);

  /** Retrieve global by name. Because of MRI, the leading $ is optional but recommended. */
  VALUE   rb_gv_get(const char* name);

  /** Convert string to an ID */
  ID      rb_intern(const char* string);

  /** Get object's instance variable. */
  VALUE   rb_ivar_get(VALUE obj_handle, ID ivar_name);

  /** Set object's instance variable to given value. */
  VALUE   rb_ivar_set(VALUE obj_handle, ID ivar_name, VALUE value);

  /** Allocate uninitialised instance of given class. */
  VALUE   rb_obj_alloc(VALUE klass);

  /** Call #to_s on object. */
  VALUE   rb_obj_as_string(VALUE obj_handle);

  /** Raise error of given class using formatted message. @todo Implement for real. */
  void    rb_raise(VALUE error_handle, const char* format_string, ...);

  /** 1 if obj.respond_to? method_name evaluates true, 0 otherwise. */
  int     rb_respond_to(VALUE obj_handle, ID method_name);

  /** Raise error if $SAFE is not higher than the given level. */
  void    rb_secure(int level);

  /** Return character at offset in String. Out-of-bounds returns '\0'. */
  char    rb_str_get_char(VALUE self_handle, int offset);

  /** Return number of bytes in String. @todo MRI returns int, problem? */
  size_t  rb_str_get_char_len(VALUE self_handle);

  /** Returns a _copy_ of the C string contained in given String. */
  char*   rb_str_get_char_ptr(VALUE str_handle);

  /** Create a String using the designated length of given C string. */
  VALUE   rb_str_new(const char* string, size_t length);

  /** Create a String from a C string. */
  VALUE   rb_str_new2(const char* string);

  /** Call #to_s on object pointed to. */
  VALUE   rb_string_value(VALUE* obj_handle);

  /** Create a String from the C string. Rubinius does not implement tainting. */
  VALUE   rb_tainted_str_new2(const char* str);

  /** Issue a thread.pass. */
  void    rb_thread_schedule();

  /** Call #to_sym on object. */
  ID      rb_to_id(VALUE obj);

  /** Call block with given argument or raise error if no block given. */
  VALUE   rb_yield(VALUE argument_handle);


#ifdef __cplusplus
}
#endif

#endif

