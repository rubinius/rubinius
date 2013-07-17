#ifndef RBX_CAPI_RUBY_RUBY_H
#define RBX_CAPI_RUBY_RUBY_H

/**
 *
 *  Notes:
 *
 *    - The function prefix capi_* is used for functions that implement
 *      the Ruby C-API but should NEVER be used in a C extension's code.
 *
 *      Just in case, that means NEVER, like NOT EVER. If you do, we'll
 *      call your mother.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef RBX_WINDOWS
#include <winsock2.h>
#else
#include <sys/select.h>
#endif

// A number of extensions expect these to be already included
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "mri_oop.h"
#include "capi_constants.h"
#include "intern.h"
#include "defines.h"

/* There are C-exts like pg and mysql that are using this define to dectect
 * whether they are running in 1.9 mode. This is extremely broken, but instead
 * of trying to chase down all those libraries now, we'll see how far this
 * gets us. It shouldn't cause issues because any C-exts that use MRI
 * internals would not compile regardless of this define.
 */
#define RUBY_VM 1 /* Fake YARV */

#define RUBY_COMPAT_LEVEL 2

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

#if SIZEOF_LONG_LONG > 0
# define LONG_LONG long long
#endif

#if SIZEOF_LONG == SIZEOF_VOIDP
# define SIGNED_VALUE long
# define SIZEOF_VALUE SIZEOF_LONG
# define PRI_VALUE_PREFIX "l"
#elif SIZEOF_LONG_LONG == SIZEOF_VOIDP
# define SIGNED_VALUE LONG_LONG
# define LONG_LONG_VALUE 1
# define SIZEOF_VALUE SIZEOF_LONG_LONG
# define PRI_VALUE_PREFIX "ll"
#else
# error ---->> ruby requires sizeof(void*) == sizeof(long) to be compiled. <<----
#endif

#ifndef RUBY_EXTERN
#define RUBY_EXTERN extern
#endif

#ifndef EXTERN
#define EXTERN RUBY_EXTERN  /* deprecated */
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

#define rb_isascii(c) ((unsigned long)(c) < 128)
int rb_isalnum(int c);
int rb_isalpha(int c);
int rb_isblank(int c);
int rb_iscntrl(int c);
int rb_isdigit(int c);
int rb_isgraph(int c);
int rb_islower(int c);
int rb_isprint(int c);
int rb_ispunct(int c);
int rb_isspace(int c);
int rb_isupper(int c);
int rb_isxdigit(int c);
int rb_tolower(int c);
int rb_toupper(int c);

#ifndef ISPRINT
#define ISASCII(c) rb_isascii((unsigned char)(c))
#undef ISPRINT
#define ISPRINT(c) rb_isprint((unsigned char)(c))
#define ISSPACE(c) rb_isspace((unsigned char)(c))
#define ISUPPER(c) rb_isupper((unsigned char)(c))
#define ISLOWER(c) rb_islower((unsigned char)(c))
#define ISALNUM(c) rb_isalnum((unsigned char)(c))
#define ISALPHA(c) rb_isalpha((unsigned char)(c))
#define ISDIGIT(c) rb_isdigit((unsigned char)(c))
#define ISXDIGIT(c) rb_isxdigit((unsigned char)(c))
#endif
#define TOUPPER(c) rb_toupper((unsigned char)(c))
#define TOLOWER(c) rb_tolower((unsigned char)(c))

/**
 * In MRI, RUBY_DATA_FUNC is used for the mark and free functions in
 * Data_Wrap_Struct and Data_Make_Struct.
 */
typedef void (*RUBY_DATA_FUNC)(void*);

#undef ALLOC
#undef ALLOC_N
#undef ALLOCA_N
#undef REALLOC_N

#define RUBY_METHOD_FUNC(func) ((VALUE (*)(ANYARGS))func)

/** Global class/object etc. types. */

#ifndef RBX_WINDOWS
  extern int __X_rubinius_version __attribute__((weak, visibility ("default")));
  int __X_rubinius_version = 2;
#endif

  /**
   *  Integral type map for MRI's types.
   *
   *  Rubinius does not implement all of these,
   *  so T_OBJECT is returned instead in those
   *  cases.
   */

#define T_ARRAY  0x00
#define T_NONE   0x01
#define T_NIL    0x02
#define T_OBJECT 0x03
#define T_CLASS  0x04
#define T_ICLASS 0x05
#define T_MODULE 0x06
#define T_FLOAT  0x07
#define T_STRING 0x08
#define T_REGEXP 0x09
#define T_FIXNUM 0x0a
#define T_HASH   0x0b
#define T_STRUCT 0x0c
#define T_BIGNUM 0x0d
#define T_FILE   0x0e

#define T_TRUE   0x0f
#define T_FALSE  0x10
#define T_DATA   0x11
#define T_MATCH  0x12
#define T_SYMBOL 0x13

#define T_BLKTAG 0x14
#define T_UNDEF  0x15
#define T_VARMAP 0x16
#define T_SCOPE  0x17
#define T_NODE   0x18

#define T_MASK   0x19

#define T_RATIONAL 0x20
#define T_COMPLEX  0x21

#define T_ENCODING 0x22

struct RString {
  ssize_t len;
  char *ptr;
  char *dmwmb;
  struct {
    ssize_t capa;
    VALUE shared;
  } aux;
};

#define RSTRING_CACHE_UNSAFE    1
#define RSTRING_CACHE_SAFE      2

#ifdef RSTRING_NOT_MODIFIED
  /* Define this macro if the C extension never modifies, but
   * only reads from, RSTRING(str)->ptr and RSTRING(str)->len.
   */
#define RSTRING(str)    capi_rstring_struct(str, RSTRING_CACHE_SAFE)
#else
  /* The default is to update the string when RSTRING(str)->len is
   * modified. We raise an exception if RSTRING(str)->ptr is changed.
   */
#define RSTRING(str)    capi_rstring_struct(str, RSTRING_CACHE_UNSAFE)
#endif

struct RArray {
  ssize_t len;
  struct {
    ssize_t capa;
    VALUE shared;
  } aux;
  VALUE *ptr;
  VALUE *dmwmb;
};

#define RARRAY(ary)     capi_rarray_struct(ary)

struct RData {
  void (*dmark)(void*);
  void (*dfree)(void*);
  void *data;
};

#define RDATA(d)        capi_rdata_struct(d)

struct RFloat {
  double value;
};

#define RFLOAT(d)       capi_rfloat_struct(d)
#define RFLOAT_VALUE(d) RFLOAT(d)->value

// Do not define these messages as strings. We want a syntax error.
#define RHASH(obj)      ({ C_API_RHASH_is_not_supported_in_Rubinius })
#define RHASH_TBL(obj)  ({ C_API_RHASH_TBL_is_not_supported_in_Rubinius })

typedef struct rb_io_t {
  VALUE handle;
  int fd;
  FILE* f;
  FILE* f2;
  FILE* stdio_file;
  void (*finalize)(struct rb_io_t*,int);
} rb_io_t;


#define RIO       rb_io_t

#define OpenFile  rb_io_t

struct RFile {
  VALUE handle;
  rb_io_t *fptr;
};

#define RFILE(obj)      capi_rfile_struct(obj)

#define HAVE_RB_IO_T 1

// Fake it out, just make the ptr be the val
// MRI checks also that it's not closed...
#define GetOpenFile(val, ptr) (ptr) = (capi_rio_struct(val))
#define rb_stdin              rb_const_get(rb_cObject, rb_intern("STDIN"))
#define rb_stdout             rb_const_get(rb_cObject, rb_intern("STDOUT"))
#define rb_stderr             rb_const_get(rb_cObject, rb_intern("STDERR"))

#define GetReadFile(ptr)  (ptr->f)
#define GetWriteFile(ptr) (ptr->f)

/* MRI Globals */
#define ruby_verbose (*(mri_global_verbose()))
#define ruby_debug   (*(mri_global_debug()))

#define rb_rs           mri_global_rb_rs()
#define rb_default_rs   mri_global_rb_default_rs()
#define rb_output_rs    mri_global_rb_output_rs()
#define rb_output_fs    mri_global_rb_output_fs()

/* Global Class objects */

#define rb_cArray             (capi_get_constant(cCApiArray))
#define rb_cBignum            (capi_get_constant(cCApiBignum))
#define rb_cClass             (capi_get_constant(cCApiClass))
#define rb_cData              (capi_get_constant(cCApiData))
#define rb_cFalseClass        (capi_get_constant(cCApiFalse))
#define rb_cFile              (capi_get_constant(cCApiFile))
#define rb_cFixnum            (capi_get_constant(cCApiFixnum))
#define rb_cFloat             (capi_get_constant(cCApiFloat))
#define rb_cHash              (capi_get_constant(cCApiHash))
#define rb_cInteger           (capi_get_constant(cCApiInteger))
#define rb_cIO                (capi_get_constant(cCApiIO))
#define rb_cMatch             (capi_get_constant(cCApiMatch))
#define rb_cModule            (capi_get_constant(cCApiModule))
#define rb_cNilClass          (capi_get_constant(cCApiNil))
#define rb_cNumeric           (capi_get_constant(cCApiNumeric))
#define rb_cObject            (capi_get_constant(cCApiObject))
#define rb_cRange             (capi_get_constant(cCApiRange))
#define rb_cRegexp            (capi_get_constant(cCApiRegexp))
#define rb_mRubinius          (capi_get_constant(cCApiRubinius))
#define rb_cString            (capi_get_constant(cCApiString))
#define rb_cStruct            (capi_get_constant(cCApiStruct))
#define rb_cSymbol            (capi_get_constant(cCApiSymbol))
#define rb_cThread            (capi_get_constant(cCApiThread))
#define rb_cTime              (capi_get_constant(cCApiTime))
#define rb_cTrueClass         (capi_get_constant(cCApiTrue))
#define rb_cProc              (capi_get_constant(cCApiProc))
#define rb_cMethod            (capi_get_constant(cCApiMethod))
#define rb_cRational          (capi_get_constant(cCApiRational))
#define rb_cComplex           (capi_get_constant(cCApiComplex))
#define rb_cEncoding          (capi_get_constant(cCApiEncoding))
#define rb_cEnumerator        (capi_get_constant(cCApiEnumerator))
#define rb_cMutex             (capi_get_constant(cCApiMutex))

/* Global Module objects. */

#define rb_mComparable        (capi_get_constant(cCApiComparable))
#define rb_mEnumerable        (capi_get_constant(cCApiEnumerable))
#define rb_mKernel            (capi_get_constant(cCApiKernel))
#define rb_mGC                (capi_get_constant(cCApiGC))
#define rb_mWaitReadable      (capi_get_constant(cCApiWaitReadable))
#define rb_mWaitWritable      (capi_get_constant(cCApiWaitWritable))

/* Utility modules */
#define rb_mCAPI              (capi_get_constant(cCApiCAPI))

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
#define rb_eMathDomainError   (capi_get_constant(cCApiMathDomainError))
#define rb_eEncCompatError    (capi_get_constant(cCApiEncCompatError))

/* Interface macros */

/** Allocate memory for type. Must NOT be used to allocate Ruby objects. */
#define ALLOC(type)       (type*)malloc(sizeof(type))

/** Allocate memory for N of type. Must NOT be used to allocate Ruby objects. */
#define ALLOC_N(type, n)  (type*)malloc(sizeof(type) * (n))

/** Allocate memory for N of type in the stack frame of the caller. */
#define ALLOCA_N(type,n)  (type*)alloca(sizeof(type)*(n))

/** Reallocate memory allocated with ALLOC or ALLOC_N. */
#define REALLOC_N(ptr, type, n) (ptr)=(type*)realloc(ptr, sizeof(type) * (n));

/** Interrupt checking (no-op). */
#define CHECK_INTS        /* No-op */

/** Rubinius doesn't need gc guards */
#define RB_GC_GUARD       /* No-op */

#define POSFIXABLE(f)     ((f) <= FIXNUM_MAX)
#define NEGFIXABLE(f)     ((f) >= FIXNUM_MIN)
#define FIXABLE(f)        (POSFIXABLE(f) && NEGFIXABLE(f))

/** Convert a Fixnum to a long int. */
#define FIX2LONG(x)       (((long)(x)) >> 1)

/** Convert a Fixnum to an unsigned long int. */
#define FIX2ULONG(x)      (((unsigned long)(x))>>1)

/** Convert a VALUE into a long int. */
#define NUM2LONG(x)       (FIXNUM_P(x)?FIX2LONG(x):rb_num2long((VALUE)x))

/** Convert a VALUE into an unsigned long int. */
#define NUM2ULONG(x)      rb_num2ulong((VALUE)x)

/** Convert a VALUE into an int. */
#define NUM2INT(x)        ((int)NUM2LONG(x))

/** Convert a VALUE into an unsigned int. */
#define NUM2UINT(x)       ((unsigned int)NUM2ULONG(x))

/** Convert a Fixnum into an int. */
#define FIX2INT(x)        ((int)FIX2LONG(x))

/** Convert a Fixnum into an unsigned int. */
#define FIX2UINT(x)       ((unsigned int)FIX2ULONG(x))

/** Get a handle for the Symbol object represented by ID. */
#define ID2SYM(id)        (id)

/** Taint/untrust o1 if o2 is tainted/untrusted. */
#define OBJ_INFECT(o1, o2) capi_infect((o1), (o2))

/** Taints the object */
#define OBJ_TAINT(obj)    capi_taint((obj))

/** Returns 1 if the object is tainted, 0 otherwise. */
#define OBJ_TAINTED(obj)  rb_obj_tainted((obj))

/** Convert int to a Ruby Integer. */
#define INT2FIX(i)        CAPI_TAG_FIXNUM(i)

/** Convert a char to a Ruby Integer. */
#define CHR2FIX(x)        INT2FIX((long)((x)&0xff))

/** Convert long to a Ruby Integer. */
#define LONG2FIX(i)       INT2FIX(i)

char rb_num2chr(VALUE);
#define NUM2CHR(x)        rb_num2chr((VALUE)x)

long long rb_num2ll(VALUE);
unsigned long long rb_num2ull(VALUE);
#define NUM2LL(x) (FIXNUM_P(x)?FIX2LONG(x):rb_num2ll((VALUE)x))
#define NUM2ULL(x) rb_num2ull((VALUE)x)
#define NUM2OFFT(x) ((off_t)NUM2LL(x))

#define OFFT2NUM(x) LL2NUM((long long)x)

#if SIZEOF_SIZE_T > SIZEOF_LONG
# define NUM2SIZET(x) ((size_t)NUM2ULL(x))
# define NUM2SSIZET(x) ((size_t)NUM2LL(x))
#else
# define NUM2SIZET(x) NUM2ULONG(x)
# define NUM2SSIZET(x) NUM2LONG(x)
#endif

#if SIZEOF_SIZE_T > SIZEOF_LONG
# define SIZET2NUM(v) ULL2NUM(v)
# define SSIZET2NUM(v) LL2NUM(v)
#elif SIZEOF_SIZE_T == SIZEOF_LONG
# define SIZET2NUM(v) ULONG2NUM(v)
# define SSIZET2NUM(v) LONG2NUM(v)
#else
# define SIZET2NUM(v) UINT2NUM(v)
# define SSIZET2NUM(v) INT2NUM(v)
#endif

#if SIZEOF_INT < SIZEOF_LONG
int rb_long2int(long n);
#else
#define rb_long2int(n)  ((int)(n))
#endif


/** Convert from a Float to a double */
double rb_num2dbl(VALUE);
#define NUM2DBL(x) rb_num2dbl((VALUE)(x))

VALUE rb_int2big(long number);
VALUE rb_uint2big(unsigned long number);

/** Zero out N elements of type starting at given pointer. */
#define MEMZERO(p,type,n) memset((p), 0, (sizeof(type) * (n)))

/** Copies n objects of type from p2 to p1. Behavior is undefined if objects
 * overlap.
 */
#define MEMCPY(p1,p2,type,n) memcpy((p1), (p2), sizeof(type)*(n))

/** Copies n objects of type from p2 to p1. Objects may overlap. */
#define MEMMOVE(p1,p2,type,n) memmove((p1), (p2), sizeof(type)*(n))

/** Compares n objects of type. */
#define MEMCMP(p1,p2,type,n) memcmp((p1), (p2), sizeof(type)*(n))

/** The length of the array. */
#define RARRAY_LEN(ary)   rb_ary_size(ary)
#define RARRAY_LENINT(str) rb_ary_size(str)

/** The pointer to the array's data. */
#define RARRAY_PTR(ary)   (RARRAY(ary)->ptr)

/** The source pattern for a regex. */
#define RREGEXP_SRC(reg)  rb_reg_source(reg)

/** The options of the regex. */
#define RREGEXP_OPTIONS(reg) rb_reg_options(reg)

/** The length of string str. */
#define RSTRING_LEN(str)    rb_str_len(str)
#define RSTRING_LENINT(str) rb_str_len(str)

/** The pointer to the string str's data. */
#ifdef RUBY_READONLY_STRING
#define RSTRING_PTR(str)  rb_str_ptr_readonly(str)
#define RSTRING_END(str)  rb_str_ptr_readonly_end(str)
#else
#define RSTRING_PTR(str)  (RSTRING(str)->ptr)
#define RSTRING_END(str)  (RSTRING(str)->ptr + RSTRING(str)->len)
#endif

/** The pointer to the data. */
#define DATA_PTR(d)       (RDATA(d)->data)

/** Return true if expression is not Qfalse or Qnil. */
#define RTEST(v)          (((VALUE)(v) & ~Qnil) != 0)

/** Return the super class of the object */
#define RCLASS_SUPER(klass)   capi_class_superclass((klass))

/** Rubinius' SafeStringValue is the same as StringValue. */
#define SafeStringValue   StringValue

/** Return true if expression is an immediate, Qfalse or Qnil. */
#define SPECIAL_CONST_P(x)    (IMMEDIATE_P(x) || !RTEST(x))

/** Modifies the VALUE object in place by calling rb_obj_as_string(). */
#define StringValue(v)        rb_string_value(&(v))
#define StringValuePtr(v)     rb_string_value_ptr(&(v))
#define StringValueCStr(str)  rb_string_value_cstr(&(str))
#define STR2CSTR(str)         rb_str2cstr((VALUE)(str), 0)

#define Check_SafeStr(x)

#define FilePathValue(v)      rb_file_path_value(&(v))

/** Retrieve the ID given a Symbol handle. */
#define SYM2ID(sym)       (sym)

/** Return an integer type id for the object. @see rb_type() */
#define TYPE(handle)      rb_type(handle)
#define rb_type_p(obj, type) (rb_type(obj) == (type))
#define RB_TYPE_P(obj, type) rb_type_p(obj, type)

/** Alias to rb_type. This is not exactly the same as in MRI, but it makes sure
+ * that it won't segfault if you give BUILTIN_TYPE an immediate such as a Fixnum
+ */
#define BUILTIN_TYPE(handle) rb_type(handle)

/** Convert unsigned int to a Ruby Integer. */
#define UINT2FIX(i)       UINT2NUM((i))

#define LONG2FIX(i) INT2FIX(i)

#define LL2NUM(val) rb_ll2inum(val)
#define ULL2NUM(val) rb_ull2inum(val)

  VALUE rb_ll2inum(long long val);
  VALUE rb_ull2inum(unsigned long long val);

#if SIZEOF_TIME_T == 8
#define TIMET2NUM(v)   LL2NUM(v)
#define NUM2TIMET(v)   NUM2LL(v)
#else
#define TIMET2NUM(v)   LONG2NUM(v)
#define NUM2TIMET(v)   NUM2LONG(v)
#endif


/* Secret extra stuff */

  typedef VALUE (*CApiAllocFunction)(VALUE);
  typedef VALUE (*CApiGenericFunction)(ANYARGS);


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

  /** Retrieve a Handle to a globally available object. @internal. */
  VALUE   capi_get_constant(CApiConstant type);

  /** Returns the string associated with a symbol. */
  const char *rb_id2name(ID sym);

  /** Returns the Ruby string associated with the symbol */
  VALUE rb_id2str(ID sym);

  /** Infect obj2 if obj1 is tainted. @internal.*/
  void    capi_infect(VALUE obj1, VALUE obj2);

  /** False if expression evaluates to nil, true otherwise. @internal. */
  int     capi_nil_p(VALUE expression_result);

  /** Taints obj. @internal. */
  void    capi_taint(VALUE obj);

  /** Returns 1 if obj is tainted, 0 otherwise. @internal. */
  int     rb_obj_tainted(VALUE obj);

  /** Builds a string based stack trace */
  VALUE rb_make_backtrace();

  /** Returns the superclass of klass or NULL. This is not the same as
   * rb_class_superclass. See MRI's rb_class_s_alloc which returns a
   * class created with rb_class_boot(0), i.e. having a NULL superclass.
   * RCLASS_SUPER(klass) is used in a boolean context to exit a loop in
   * the Digest extension. It's likely other extensions do the same thing.
   */
  VALUE   capi_class_superclass(VALUE klass);

  struct RArray* capi_rarray_struct(VALUE array);
  struct RData* capi_rdata_struct(VALUE data);
  struct RString* capi_rstring_struct(VALUE string, int cache_level);
  struct RFloat* capi_rfloat_struct(VALUE data);
  struct RIO* capi_rio_struct(VALUE handle);
  struct RFile* capi_rfile_struct(VALUE file);

/* Real API */

  /** Convert a VALUE into a long int. */
  long    rb_num2long(VALUE obj);

  /** Convert a VALUE to an unsigned long int. */
  unsigned long rb_num2ulong(VALUE obj);

  /** Convert a long int into an Integer. */
  VALUE rb_int2inum(long n);
#define rb_int_new(v) rb_int2inum(v)
  VALUE INT2NUM(long n);
  VALUE LONG2NUM(long n);

  /** Convert a unsigned long int into an Integer. */
  VALUE rb_uint2inum(unsigned long n);
  VALUE UINT2NUM(unsigned long n);
  VALUE ULONG2NUM(unsigned long n);

  int   rb_cmpint(VALUE val, VALUE a, VALUE b);
  void  rb_cmperr(VALUE x, VALUE y);
  VALUE rb_equal(VALUE a, VALUE b);
  VALUE rb_class_inherited_p(VALUE mod, VALUE arg);

#define   Data_Make_Struct(klass, type, mark, free, sval) (\
            sval = ALLOC(type), \
            memset(sval, 0, sizeof(type)), \
            Data_Wrap_Struct(klass, mark, free, sval)\
          )

#define   Data_Wrap_Struct(klass, mark, free, sval) \
            rb_data_object_alloc(klass, (void*)sval, (RUBY_DATA_FUNC)mark, \
                                 (RUBY_DATA_FUNC)free)

#define   Data_Get_Struct(obj,type,sval) do {\
            Check_Type(obj, T_DATA); \
            sval = (type*)DATA_PTR(obj);\
} while (0)

  /** Return Qtrue if obj is an immediate, Qfalse or Qnil. */
  int     rb_special_const_p(VALUE obj);

  /** Return Qtrue if sym is suitable as a name of a constant. */
  int     rb_is_const_id(ID sym);

  /** Return Qtrue if sym is suitable as a name of an instance variable. */
  int     rb_is_instance_id(ID sym);

  /** Return Qtrue if sym is suitable as a name of a class variable. */
  int     rb_is_class_id(ID sym);

  /** Return obj if it is an Array, or return wrapped (i.e. [obj]) */
  VALUE   rb_Array(VALUE object);

  /** Remove all elements from the Array. Returns self. */
  VALUE   rb_ary_clear(VALUE self);

  /** Remove and return an element from the Array. */
  VALUE   rb_ary_delete(VALUE self, VALUE item);

  /** Remove and return the element at index from the Array. */
  VALUE   rb_ary_delete_at(VALUE self, long idx);

  /** Return shallow copy of the Array. The elements are not dupped. */
  VALUE   rb_ary_dup(VALUE self);

  /** Return object at index. Out-of-bounds access returns Qnil. */
  VALUE   rb_ary_entry(VALUE self, int index);

  /** Return Qtrue if the array includes the item. */
  VALUE   rb_ary_includes(VALUE self, VALUE obj);

  /** Array#join. Returns String with all elements to_s, with optional separator String. */
  VALUE   rb_ary_join(VALUE self, VALUE separator);

  /** Array#to_s. Returns String with all elements to_s, without a separator string */
  VALUE   rb_ary_to_s(VALUE self);

  /** New, empty Array. */
  VALUE   rb_ary_new();

  /** New Array of nil elements at given length. */
  VALUE   rb_ary_new2(unsigned long length);

  /** New Array of given length, filled with varargs elements. */
  VALUE   rb_ary_new3(unsigned long length, ...);

  /** New Array of given length, filled with copies of given object. */
  VALUE   rb_ary_new4(unsigned long length, const VALUE* object);

  /** Remove and return last element of Array or nil. */
  VALUE   rb_ary_pop(VALUE self);

  /** Appends value to end of Array and returns self. */
  VALUE   rb_ary_push(VALUE self, VALUE object);

  /** Returns a new Array with elements in reverse order. Elements not dupped. */
  VALUE   rb_ary_reverse(VALUE self);

  /** Remove and return first element of Array or nil. Changes other elements' indexes. */
  VALUE   rb_ary_shift(VALUE self);

  /** Number of elements in given Array. */
  long    rb_ary_size(VALUE self);

  /** Store object at given index. Supports negative indexes. Returns object. */
  void    rb_ary_store(VALUE self, long int index, VALUE object);

  /** Concat two arrays */
  VALUE   rb_ary_concat(VALUE self, VALUE second);

  /** Add object to the front of Array. Changes old indexes +1. Returns object. */
  VALUE   rb_ary_unshift(VALUE self, VALUE object);

  /** Returns the element at index, or returns a subarray or returns a subarray specified by a range. */
  VALUE   rb_ary_aref(int argc, VALUE *argv, VALUE object);

  VALUE   rb_ary_each(VALUE ary);

  /** Same as rb_obj_freeze */
  VALUE   rb_ary_freeze(VALUE ary);

  /** Array coercion method */
  VALUE   rb_ary_to_ary(VALUE ary);

  void    rb_mem_clear(VALUE* ary, int len);

  /** Return new Array with elements first and second. */
  VALUE   rb_assoc_new(VALUE first, VALUE second);

  /** @see rb_ivar_get */
  VALUE   rb_attr_get(VALUE object, ID attr_name);

  /** Return Array with names of object's instance variables */
  VALUE   rb_obj_instance_variables(VALUE object);

  void    rb_attr(VALUE klass, ID id, int read, int write, int ex);

  /** Return 1 if this send has a block, 0 otherwise. */
  int     rb_block_given_p();

  /* raises a LocalJumpError if no block was given */
  void    rb_need_block();

  /* Converts implicit block into a new Proc. */
  VALUE   rb_block_proc();

  typedef VALUE rb_block_call_func(VALUE, VALUE, int, VALUE*);
  VALUE rb_block_call(VALUE,ID,int,VALUE*,VALUE(*)(ANYARGS),VALUE);
#define HAVE_RB_BLOCK_CALL 1

  VALUE   rb_each(VALUE);

  VALUE   rb_iterate(VALUE (*ifunc)(VALUE), VALUE ary, VALUE(*cb)(ANYARGS), VALUE cb_data);

  VALUE   rb_big2str(VALUE self, int base);

  long    rb_big2long(VALUE obj);

  unsigned long rb_big2ulong(VALUE obj);

  long long rb_big2ll(VALUE obj);

  unsigned long long rb_big2ull(VALUE);

  double  rb_big2dbl(VALUE obj);

  int     rb_big_bytes_used(VALUE obj);

  int rb_big_sign(VALUE obj);
#define RBIGNUM_SIGN(obj) rb_big_sign(obj)
#define RBIGNUM_POSITIVE_P(b) RBIGNUM_SIGN(b)
#define RBIGNUM_NEGATIVE_P(b) (!RBIGNUM_SIGN(b))

#if SIZEOF_INT*2 <= SIZEOF_LONG_LONG
# define BDIGIT unsigned int
# define SIZEOF_BDIGITS SIZEOF_INT
# define BDIGIT_DBL unsigned LONG_LONG
# define BDIGIT_DBL_SIGNED LONG_LONG
#elif SIZEOF_INT*2 <= SIZEOF_LONG
# define BDIGIT unsigned int
# define SIZEOF_BDIGITS SIZEOF_INT
# define BDIGIT_DBL unsigned long
# define BDIGIT_DBL_SIGNED long
#elif SIZEOF_SHORT*2 <= SIZEOF_LONG
# define BDIGIT unsigned short
# define SIZEOF_BDIGITS SIZEOF_SHORT
# define BDIGIT_DBL unsigned long
# define BDIGIT_DBL_SIGNED long
#else
# define BDIGIT unsigned short
# define SIZEOF_BDIGITS (SIZEOF_LONG/2)
# define BDIGIT_DBL unsigned long
# define BDIGIT_DBL_SIGNED long
#endif

#define RBIGNUM_LEN(obj) (rb_big_bytes_used(obj) / SIZEOF_BDIGITS)
  /** Calls this method in a superclass. */
  VALUE rb_call_super(int argc, const VALUE *argv);

  /** If object responds to #to_ary, returns the result of that call, otherwise nil. */
  VALUE   rb_check_array_type(VALUE object);

  /** If object responds to #to_str, returns the result of that call, otherwise nil. */
  VALUE   rb_check_string_type(VALUE object);

  /** If object responds to the given method, returns the result of that call
   * when an integer, otherwise nil. */
  VALUE   rb_check_to_integer(VALUE object, const char* method_name);

  /** Raises an exception if object is frozen. */
  void    rb_check_frozen(VALUE object);

  /** check if obj is frozen. */
  VALUE     rb_obj_frozen_p(VALUE obj);
#define OBJ_FROZEN(obj) (RTEST(rb_obj_frozen_p(obj)))

  /** raise error on class */
  NORETURN(void rb_error_frozen(const char* what));

  /** Raises an exception if object is not the same type as 'type'. */
  void    rb_check_type(VALUE object, int type);

#define Check_Type(v,t) rb_check_type((VALUE)(v),(t))

  /**
   *  Safe type conversion.
   *
   *  If the object responds to the given method name, the method is
   *  called and the result returned. Otherwise returns nil.
   *
   *  @see rb_check_array_type() and rb_check_string_type().
   */
  VALUE   rb_check_convert_type(VALUE object, int type,
      const char* type_name, const char* method_name);

  void    rb_check_safe_obj(VALUE obj);

  void    rb_check_safe_str(VALUE obj);

  void    rb_secure_update(VALUE obj);

  /** Returns String representation of the class' name. */
  VALUE   rb_class_name(VALUE klass);

  /** Calls the class method 'inherited' on super passing the class.
   *  If super is NULL, calls Object.inherited. */
  VALUE   rb_class_inherited(VALUE super, VALUE klass);

  /** Returns a new, anonymous class inheriting from super.
   *  Does NOT call inherited() on the superclass. */
  VALUE   rb_class_new(VALUE super);

  /** As Ruby's .new, with the given arguments. Returns the new object. */
  VALUE   rb_class_new_instance(int arg_count, VALUE* args, VALUE klass);

  /** Returns the Class object this object is an instance of. */
  VALUE   rb_class_of(VALUE object);

  /** Returns the superclass of a class. */
  VALUE   rb_class_superclass(VALUE klass);

  /** Returns the Class object contained in the klass field of object
   * (ie, a singleton class if it's there) */
  VALUE   CLASS_OF(VALUE object);

  /** C string representation of the class' name. */
  const char*   rb_class2name(VALUE klass);

  /** Return the module referred to by qualified path (e.g. A::B::C) */
  VALUE   rb_path2class(const char*);

  /** Return the module referred to by qualified path (e.g. A::B::C) */
  VALUE   rb_path_to_class(VALUE str);

  /** Print the value to $stdout */
  void    rb_p(VALUE);

  /** Return an array containing the names of all global variables */
  VALUE   rb_f_global_variables();

  /** Returns object returned by invoking method on object if right type, or raises error. */
  VALUE   rb_convert_type(VALUE object, int type,
      const char* type_name, const char* method_name);

  /** Nonzero if constant corresponding to Symbol exists in the Module. */
  int     rb_const_defined(VALUE module, ID const_id);

  /** Returns non-zero if the constant is defined in the module. Does
   *  not search outside of the module itself. */
  int     rb_const_defined_at(VALUE module, ID const_id);

  /** Retrieve constant from given module. */
  VALUE   rb_const_get(VALUE module, ID id_name);

  /** Returns a constant defined in module only. Does not search
   *  outside of the module itself. */
  VALUE   rb_const_get_at(VALUE module, ID id_name);

  /** Retrieve constant from given module. */
  VALUE rb_const_get_from(VALUE module, ID id_name);

  /** Set constant on the given module */
  void rb_const_set(VALUE module, ID name, VALUE constant);

  VALUE rb_mod_remove_const(VALUE mod, VALUE name);
  VALUE rb_mod_ancestors(VALUE mod);
  VALUE rb_mod_name(VALUE mod);
  VALUE rb_module_new(void);

  /** Parses a string into a double value. If badcheck is true, raises an
   * exception if the string contains non-digit or '.' characters.
   */
  double rb_cstr_to_dbl(const char *p, int badcheck);

  /** Return Integer obtained from String#to_i using given base. */
  VALUE   rb_cstr2inum(const char* string, int base);
  VALUE   rb_cstr_to_inum(const char* str, int base, int badcheck);

  /** Returns module's named class variable. @@ is optional. */
  VALUE   rb_cv_get(VALUE module, const char* name);

  /** Set module's named class variable to given value. Returns the value. @@ is optional. */
  VALUE   rb_cv_set(VALUE module, const char* name, VALUE value);

  /** Returns a value evaluating true if module has named class var. @@ is optional. */
  VALUE   rb_cvar_defined(VALUE module, ID name);

  /** Returns class variable by (Symbol) name from module. @@ is optional. */
  VALUE   rb_cvar_get(VALUE module, ID name);

  /** Set module's named class variable to given value. Returns the value. @@ is optional. */
  VALUE   rb_cvar_set_internal(VALUE module, ID name, VALUE value);
#define rb_cvar_set   rb_cvar_set_internal

  /** Set module's named class variable to given value. */
  void rb_define_class_variable(VALUE klass, const char* name, VALUE val);

  VALUE   rb_data_object_alloc(VALUE klass, void* sval,
      RUBY_DATA_FUNC mark, RUBY_DATA_FUNC free);

  /** Alias method by old name as new name. Methods are independent of eachother. */
  void    rb_define_alias(VALUE module, const char *new_name, const char *old_name);
  void    rb_alias(VALUE module, ID id_new, ID id_old);

  /** Define an .allocate for the given class. Should take no args and return a VALUE. */
  void    rb_define_alloc_func(VALUE klass, CApiAllocFunction allocator);

  /** Undefine the .allocate for the given class. */
  void    rb_undef_alloc_func(VALUE klass);

  /** Ruby's attr_* for given name. Nonzeros to toggle read/write. */
  void    rb_define_attr(VALUE module, const char* attr_name,
      int readable, int writable);

  /** Reopen or create new top-level class with given superclass and name. Returns the Class object. */
  VALUE   rb_define_class(const char* name, VALUE superclass);

  /** Reopen or create new class with superclass and name under parent module. Returns the Class object. */
  VALUE   rb_define_class_under(VALUE parent, const char* name, VALUE superclass);

  /** Define a toplevel constant */
  void    rb_define_global_const(const char* name, VALUE obj);

  /** Define a constant in given Module's namespace. */
  void    rb_define_const(VALUE module, const char* name, VALUE object);

  /** Generate a NativeMethod to represent a method defined as a C function. Records file. */
  #define rb_define_method(mod, name, fptr, arity) \
          capi_define_method(__FILE__, (mod), (name), \
                                           (CApiGenericFunction)(fptr), (arity), \
                                           cCApiPublicMethod)

  /** Defines the method on Kernel. */
  void    rb_define_global_function(const char* name, CApiGenericFunction func, int argc);

  /** Reopen or create new top-level Module. */
  VALUE   rb_define_module(const char* name);

  /** Defines the method as a private instance method and a singleton method of module. */
  void    rb_define_module_function(VALUE module,
      const char* name, CApiGenericFunction func, int args);

  /** Reopen or create a new Module inside given parent Module. */
  VALUE   rb_define_module_under(VALUE parent, const char* name);

  /** Generate a NativeMethod to represent a private method defined in the C function. */
  #define rb_define_private_method(mod, name, fptr, arity) \
          capi_define_method(__FILE__, (mod), (name), \
                                           (CApiGenericFunction)(fptr), (arity), \
                                           cCApiPrivateMethod)

  /** Generate a NativeMethod to represent a protected method defined in the C function. */
  #define rb_define_protected_method(mod, name, fptr, arity) \
          capi_define_method(__FILE__, (mod), (name), \
                                           (CApiGenericFunction)(fptr), (arity), \
                                           cCApiProtectedMethod)

  /** Generate a NativeMethod to represent a singleton method. @see capi_define_method. */
  #define rb_define_singleton_method(mod, name, fptr, arity) \
          capi_define_method(__FILE__, (mod), (name), \
                                           (CApiGenericFunction)(fptr), (arity), \
                                           cCApiSingletonMethod)

  /** Create an Exception from a class, C string and length. */
  VALUE   rb_exc_new(VALUE etype, const char *ptr, long len);

  /** Create an Exception from a class and C string. */
  VALUE   rb_exc_new2(VALUE etype, const char *s);

  /** Create an Exception from a class and Ruby string. */
  VALUE   rb_exc_new3(VALUE etype, VALUE str);

  /** Raises passed exception handle */
  NORETURN(void rb_exc_raise(VALUE exception));

  /** Return the current exception */
  VALUE   rb_errinfo();
#define ruby_errinfo rb_errinfo()

  /** Set the current exception */
  void    rb_set_errinfo(VALUE err);

  // To advertise we have rb_errinfo to extensions
#define HAVE_RB_ERRINFO 1

  /** Remove a previously declared global variable. */
  void    rb_free_global(VALUE global);

  /** Freeze object and return it. */
  VALUE   rb_obj_freeze(VALUE obj);
#define OBJ_FREEZE(obj) (rb_obj_freeze(obj))

  /** Taint an object and return it */
  VALUE rb_obj_taint(VALUE obj);

  /** Returns Method object for given method name */
  VALUE rb_obj_method(VALUE self, VALUE method);

  /** Returns a string formatted with Kernel#sprintf. */
  VALUE rb_f_sprintf(int argc, const VALUE* argv);

  /** Returns a File opened with the specified mode. */
  VALUE rb_file_open(const char* name, const char* mode);

  /** Returns a File opened with the specified mode. */
  VALUE rb_file_open_str(VALUE name, const char* mode);

  /** Calls #to_path or #to_str to convert an object to a String. */
  VALUE rb_file_path_value(volatile VALUE* obj);

  /**
   *  Call method on receiver, args as varargs. Calls private methods.
   */
  VALUE   rb_funcall(VALUE receiver, ID method_name,
                     int arg_count, ...);


  /** Call the method with args provided in a C array. Calls private methods. */
  VALUE   rb_funcall2(VALUE receiver, ID method_name,
                      int arg_count, const VALUE* args);

  /** Call the method with args provided in a C array and block.
   *  Calls private methods. */
  VALUE   rb_funcall2b(VALUE receiver, ID method_name, int arg_count,
                       const VALUE* v_args, VALUE block);

  /** Return name of the function being called */
  ID rb_frame_last_func();

  /** Return name of the current Ruby method */
  ID rb_frame_this_func();

  VALUE rb_exec_recursive(VALUE (*func)(VALUE, VALUE, int),
                          VALUE obj, VALUE arg);
#define HAVE_RB_EXEC_RECURSIVE 1

  /** @todo define rb_funcall3, which is the same as rb_funcall2 but
   * will not call private methods.
   */
  #define rb_funcall3 rb_funcall2

  /** Return the hash id of the object **/
  VALUE   rb_hash(VALUE self);

  /** Create a new Hash object */
  VALUE   rb_hash_new();

  /** Return the value associated with the key, including default values. */
  VALUE   rb_hash_aref(VALUE self, VALUE key);

  /** Return the value associated with the key, excluding default values. */
  VALUE   rb_hash_lookup(VALUE self, VALUE key);

  /** Set the value associated with the key. */
  VALUE   rb_hash_aset(VALUE self, VALUE key, VALUE value);

  /** Remove the key and return the associated value. */
  VALUE   rb_hash_delete(VALUE self, VALUE key);

  /** Removes the entry if the block returns true. */
  VALUE   rb_hash_delete_if(VALUE self);

  /** Returns the number of entries as a Fixnum. */
  VALUE   rb_hash_size(VALUE self);

  // macros to access the size "directly"
#define RHASH_SIZE(obj) FIX2INT(rb_hash_size(obj))
#define RHASH_LEN(val)  FIX2INT(rb_hash_size(val))

  /** Iterate over the hash, calling the function. */
  void rb_hash_foreach(VALUE self,
                       int (*func)(ANYARGS),
                       VALUE farg);

  void    rb_eof_error();

  VALUE rb_io_addstr(VALUE, VALUE);
  VALUE rb_io_printf(int, VALUE*, VALUE);
  VALUE rb_io_print(int, VALUE*, VALUE);
  VALUE rb_io_puts(int, VALUE*, VALUE);

  /** Send #write to io passing str. */
  VALUE   rb_io_write(VALUE io, VALUE str);

  /** Close an IO */
  VALUE   rb_io_close(VALUE io);

  VALUE   rb_io_binmode(VALUE io);

  int     rb_io_fd(VALUE io);
#define HAVE_RB_IO_FD 1

  int     rb_io_wait_readable(int fd);
  int     rb_io_wait_writable(int fd);

  void    rb_io_set_nonblock(rb_io_t* io);
  void    rb_io_check_closed(rb_io_t* io);
  void    rb_io_check_readable(rb_io_t* io);
  void    rb_io_check_writable(rb_io_t* io);

  FILE *  rb_io_stdio_file(rb_io_t *fptr);

  void    rb_update_max_fd(int fd);

  void    rb_thread_wait_fd(int fd);
  void    rb_thread_fd_writable(int fd);
  void    rb_thread_wait_for(struct timeval time);
#define rb_thread_create(func, arg) capi_thread_create(func, arg, #func, __FILE__)
  VALUE   capi_thread_create(VALUE (*)(ANYARGS), void*, const char* name, const char* file);

  /** Functions for mutexes, mostly used for thread safety in extensions */
  VALUE rb_mutex_new();
  VALUE rb_mutex_locked_p(VALUE mutex);
  VALUE rb_mutex_trylock(VALUE mutex);
  VALUE rb_mutex_lock(VALUE mutex);
  VALUE rb_mutex_unlock(VALUE mutex);
  VALUE rb_mutex_sleep(VALUE mutex, VALUE timeout);
  VALUE rb_mutex_synchronize(VALUE mutex, VALUE (*func)(VALUE arg), VALUE arg);

  /** Mark ruby object ptr. */
  void    rb_gc_mark(VALUE ptr);
  /** Mark a list of Ruby object ptr's. */
  void    rb_gc_mark_locations(VALUE*, VALUE*);

  /**
   * Marks an object if it is in the heap. Equivalent to rb_gc_mark in
   * Rubinius since that function checks if a handle is a GC object.
   */
  void    rb_gc_mark_maybe(VALUE ptr);

  /** Manually runs the garbage collector. */
  VALUE   rb_gc_start();

  /** Yet another way to request to run the GC */
  void    rb_gc();

  /** Request to enable GC (is always enabled in Rubinius anyway) */
  VALUE   rb_gc_enable();

  /** Request to disable GC (doesn't actually happen) */
  VALUE   rb_gc_disable();

  /** Mark variable global. Will not be GC'd. */
#define rb_global_variable(address)   capi_gc_register_address(address, __FILE__, __LINE__)
#define rb_gc_register_address(address)   capi_gc_register_address(address, __FILE__, __LINE__)
  void    capi_gc_register_address(VALUE* address, const char* file, int line);

  /** Unmark variable as global */
  void    rb_gc_unregister_address(VALUE* address);

  void    rb_gc_force_recycle(VALUE blah);

  /** Called when there is no memory available */
  void    rb_memerror();

  /** Retrieve global by name. Because of MRI, the leading $ is optional but recommended. */
  VALUE   rb_gv_get(const char* name);

  /** Set named global to given value. Returns value. $ optional. */
  VALUE   rb_gv_set(const char* name, VALUE value);

  /** Set a global name to be used to address the VALUE at addr */
  void rb_define_readonly_variable(const char* name, VALUE* addr);

  /** Include Module in another Module, just as Ruby's Module#include. */
  void    rb_include_module(VALUE includer, VALUE includee);

  /** Convert string to an ID */
  ID      rb_intern(const char* string);
  ID      rb_intern2(const char* string, long len);

  /** Coerce x and y and perform 'x func y' */
  VALUE rb_num_coerce_bin(VALUE x, VALUE y, ID func);

  /** Coerce x and y; perform 'x func y' if coerce succeeds, else return Qnil. */
  VALUE rb_num_coerce_cmp(VALUE x, VALUE y, ID func);
#define RB_NUM_COERCE_FUNCS_NEED_OPID 1

  /** Coerce x and y; perform 'x relop y' if coerce succeeds, else return Qnil. */
  VALUE rb_num_coerce_relop(VALUE x, VALUE y, ID func);

  /** Call #initialize on the object with given arguments. */
  void    rb_obj_call_init(VALUE object, int arg_count, VALUE* args);

  /** Returns the Class object this object is an instance of. */
  #define rb_obj_class(object) rb_class_of((object))

  /** String representation of the object's class' name. */
  const char*   rb_obj_classname(VALUE object);

  /** Returns true-ish if object is an instance of specific class. */
  VALUE   rb_obj_is_instance_of(VALUE object, VALUE klass);

  /** Returns true-ish if module is object's class or other ancestor. */
  VALUE   rb_obj_is_kind_of(VALUE object, VALUE module);

  /** Returns the object_id of the object. */
  VALUE   rb_obj_id(VALUE self);

  /** Return object's instance variable by name. @ optional. */
  VALUE   rb_iv_get(VALUE self, const char* name);

  /** Set instance variable by name to given value. Returns the value. @ optional. */
  VALUE   rb_iv_set(VALUE self, const char* name, VALUE value);

  /** Get object's instance variable. */
  VALUE   rb_ivar_get(VALUE object, ID ivar_name);

  /** Set object's instance variable to given value. */
  VALUE   rb_ivar_set(VALUE object, ID ivar_name, VALUE value);

  /** Checks if object has an ivar named ivar_name. */
  VALUE   rb_ivar_defined(VALUE object, ID ivar_name);

  /** Allocate uninitialised instance of given class. */
  VALUE   rb_obj_alloc(VALUE klass);


  /** Clone an instance of an object. */
  VALUE   rb_obj_dup(VALUE obj);

  /** Call #to_s on object. */
  VALUE   rb_obj_as_string(VALUE object);

  VALUE   rb_obj_instance_eval(int argc, VALUE* argv, VALUE self);

  VALUE   rb_any_to_s(VALUE obj);

  /** Return a clone of the object by calling the method bound
   * to Kernel#clone (i.e. does NOT call specialized #clone method
   * on object if one exists).
   */
  VALUE rb_obj_clone(VALUE object);

  /** Adds the module's instance methods to the object. */
  void rb_extend_object(VALUE obj, VALUE mod);

  /** Call #inspect on an object. */
  VALUE rb_inspect(VALUE object);

  /** Returns a Proc wrapping a C function. */
  VALUE rb_proc_new(VALUE (*func)(ANYARGS), VALUE val);

  VALUE rb_protect_inspect(VALUE (*func)(VALUE a, VALUE b), VALUE h_obj, VALUE h_arg);
  VALUE rb_inspecting_p(VALUE obj);

  /**
   *  Raise error of given class using formatted message.
   *
   */
  NORETURN(void rb_raise(VALUE error, const char* format_string, ...));

  /**
   *  Transfer control to the end of the innermost catch block
   *  waiting for the given symbol.
   *  The given value is returned by the catch block.
   */
  NORETURN(void rb_throw(const char* symbol, VALUE result));
  NORETURN(void rb_throw_obj(VALUE obj, VALUE result));

  /**
   * Run the function and catch the possible throw value
   */
  VALUE rb_catch(const char*, VALUE (*func)(ANYARGS), VALUE);
  VALUE rb_catch_obj(VALUE, VALUE (*func)(ANYARGS), VALUE);

  /**
   * Calls the function 'func', with arg1 as the argument.  If an exception
   * occurs during 'func', it calls 'raise_func' with arg2 as the argument.  The
   * return value of rb_rescue() is the return value from 'func' if no
   * exception occurs, from 'raise_func' otherwise.
   */
  VALUE rb_rescue(VALUE (*func)(ANYARGS), VALUE arg1, VALUE (*raise_func)(ANYARGS), VALUE arg2);

  /**
   * Same as rb_rescue() except that it also receives a list of exceptions classes.
   * It will only call 'raise_func' if the exception occurred during 'func' is a
   * kind_of? one of the passed exception classes.
   * The last argument MUST always be 0!
   */
  VALUE rb_rescue2(VALUE (*func)(ANYARGS), VALUE arg1, VALUE (*raise_func)(ANYARGS), VALUE arg2, ...);

  /**
   * Calls the function func(), with arg1 as the argument, then call ensure_func()
   * with arg2, even if func() raised an exception. The return value from rb_ensure()
   * is the return of func().
   */
  VALUE rb_ensure(VALUE (*func)(ANYARGS), VALUE arg1, VALUE (*ensure_func)(ANYARGS), VALUE arg2);

  /**
   * Call func(), and if there is an exception, returns nil and sets
   * *status to 1, otherwise the return of func is returned and *status
   * is 0.
   */
  VALUE rb_protect(VALUE (*func)(VALUE), VALUE data, int* status);

  /**
   * Break from iteration
   */
  NORETURN(void rb_iter_break(void));

  /**
   * Return the last Ruby source file in the backtrace
   */
  const char* rb_sourcefile();
#define HAVE_RB_SOURCEFILE 1

  /**
   * Return the line of the last Ruby code in the backtrace
   */
  int rb_sourceline();
#define HAVE_RB_SOURCELINE 1

  /**
   * Continue raising a pending exception if status is not 0
   */
  void rb_jump_tag(int status);

  /**
   * Retrieve the source pattern for the regular expression.
   */
  VALUE rb_reg_source(VALUE r);

  /**
   * Retrieve the pattern options for the regular expression.
   */
  int rb_reg_options(VALUE r);

  /**
   * Compile the String object into a regexp.
   */
  VALUE rb_reg_regcomp(VALUE str);

  /**
   * Tests whether the given Regexp matches to the given String
   */
  VALUE   rb_reg_match(VALUE re, VALUE str);

  /**
   * Retrieves the last MatchData
   */
  VALUE   rb_backref_get(void);

  /**
   *  Require a Ruby file.
   *
   *  Returns true on first load, false if already loaded or raises.
   */
  VALUE   rb_require(const char* name);

  /** 1 if obj.respond_to? method_name evaluates true, 0 otherwise. */
  int     rb_respond_to(VALUE object, ID method_name);

  /** 1 if obj.respond_to? method_name evaluates true, 0 otherwise. */
  int     rb_obj_respond_to(VALUE obj, ID id, int priv);

  /** 1 if class has the given method defined */
  int     rb_method_boundp(VALUE cls, ID method_name, int exclude_priv);

  /** Returns the current $SAFE level. */
  int     rb_safe_level();

  /* Marshals an object, optionally writing output to an IO */
  VALUE   rb_marshal_dump(VALUE obj, VALUE io);

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
  int     rb_scan_args_19(int argc, const VALUE* argv, const char* spec, ...);
#define rb_scan_args    rb_scan_args_19

  /** Raise error if $SAFE is not higher than the given level. */
  void    rb_secure(int level);

  /** Set $SAFE to given _higher_ level. Lowering $SAFE is not allowed. */
  void    rb_set_safe_level(int new_level);

  /** Returns the MetaClass object of the object. */
  VALUE   rb_singleton_class(VALUE object);

  /** Tries to return a String using #to_str. Error raised if no or invalid conversion. */
  VALUE   rb_String(VALUE object);

  /** Returns a Struct with the specified fields. */
  VALUE rb_struct_define(const char *name, ...);

  /** Creat an instance of a struct */
  VALUE rb_struct_new(VALUE klass, ...);

  /** Returns the value of the key. */
  VALUE rb_struct_aref(VALUE s, VALUE key);
  VALUE rb_struct_getmember(VALUE s, ID key);

  /** Sets the value of the key. */
  VALUE rb_struct_aset(VALUE s, VALUE key, VALUE value);

  /** Sets the $KCODE variable. */
  void    rb_set_kcode(const char *code);

  /** Returns a String in locale encoding. */
  VALUE rb_locale_str_new_cstr(const char* string);

  /** Returns a String in locale encoding. */
  VALUE rb_locale_str_new(const char* string, long len);

  VALUE rb_str_export(VALUE);
  VALUE rb_str_export_locale(VALUE);

#define ExportStringValue(v) do {\
    SafeStringValue(v);\
   (v) = rb_str_export(v);\
} while (0)

  /** Returns a pointer to a persistent char [] that contains the same data as
   * that contained in the Ruby string. The buffer is flushed to the string
   * when control returns to Ruby code. The buffer is updated with the string
   * contents when control crosses to C code.
   *
   * @note This is NOT an MRI C-API function.
   */
  char *rb_str_ptr(VALUE self);

  /** Returns a pointer to a persistent char [] that contains the same data as
   * that contained in the Ruby string. The buffer is intended to be
   * read-only. No changes to the buffer will be propagated to the Ruby
   * string, and no changes to the Ruby object will be propagated to the
   * buffer.  The buffer will persist as long as the Ruby string persists.
   *
   * @note This is NOT an MRI C-API function.
   */
  char* rb_str_ptr_readonly(VALUE self);
#define HAVE_RB_STR_PTR_READONLY 1

  char* rb_str_ptr_readonly_end(VALUE self);

  /** Appends other String to self and returns the modified self. */
  VALUE   rb_str_append(VALUE self, VALUE other);

  /** Appends other String to self and returns self. @see rb_str_append */
  VALUE   rb_str_buf_append(VALUE self, VALUE other);

  /** Append given number of bytes from C string to and return the String. */
  VALUE   rb_str_buf_cat(VALUE string, const char* other, size_t size);

  /** Append C string to and return the String. Uses strlen(). @see rb_str_buf_cat */
  VALUE   rb_str_buf_cat2(VALUE string, const char* other);

  /**
   *  Return new empty String with preallocated storage.
   */
  VALUE   rb_str_buf_new(long capacity);

  /** Return a new string based on the given C string */
  VALUE   rb_str_buf_new2(const char* string);

  /** Return new String concatenated of the two. */
  VALUE   rb_str_cat(VALUE string, const char* other, size_t length);

  /** Return new String concatenated of the two. Uses strlen(). @see rb_str_cat */
  VALUE   rb_str_cat2(VALUE string, const char* other);

  /** Compare Strings as Ruby String#<=>. Returns -1, 0 or 1. */
  int     rb_str_cmp(VALUE first, VALUE second);

  /** Append other String or character to self, and return the modified self. */
  VALUE   rb_str_concat(VALUE self, VALUE other);

  /** As Ruby's String#dup, returns copy of self as a new String. */
  VALUE   rb_str_dup(VALUE self);

  /** Returns an escaped String. */
  VALUE   rb_str_inspect(VALUE self);

  /** Returns a symbol created from this string. */
  VALUE   rb_str_intern(VALUE self);

  /** Returns the size of the string. It accesses the size directly and does
   * not cause the string to be cached.
   *
   * @note This is NOT an MRI C-API function.
   */
  long    rb_str_len(VALUE self);

  void    rb_str_set_len(VALUE self, size_t len);

  /** Create a String using the designated length of given C string. */
  /* length is a long because MRI has it as a long, and it also has
   * to check that length is greater than 0 properly */
  VALUE   rb_str_new(const char* string, long length);

  /** Create a String from a C string. Alias of rb_str_new2. */
  VALUE rb_str_new_cstr(const char* string);

  /** Create a String from a C string. */
  VALUE   rb_str_new2(const char* string);

  /** Create a String from an existing string. */
  VALUE   rb_str_new3(VALUE string);

  /** Create a frozen String from an existing string. */
  VALUE   rb_str_new4(VALUE string);
#define rb_str_new_frozen rb_str_new4
#define rb_str_dup_frozen rb_str_new4

  /** Creates a new String in the external encoding from a pointer and length. */
  VALUE rb_external_str_new(const char* string, long size);

  /** Creates a new String in the external encoding from a pointer. */
  VALUE rb_external_str_new_cstr(const char* string);

  void    rb_str_modify(VALUE str);

  /** Deprecated alias for rb_obj_freeze */
  VALUE   rb_str_freeze(VALUE str);

  /** Returns a new String created from concatenating self with other. */
  VALUE   rb_str_plus(VALUE self, VALUE other);

  /** Makes str at least len characters. */
  VALUE   rb_str_resize(VALUE self, size_t len);

  /** Splits self using the separator string. Returns Array of substrings. */
  VALUE   rb_str_split(VALUE self, const char* separator);

  VALUE   rb_str_subseq(VALUE self, size_t starting_index, size_t length);

  /**
   *  As Ruby's String#slice.
   *
   *  Returns new String with copy of length characters
   *  starting from the given index of self. The index
   *  may be negative. Normal String#slice border conditions
   *  apply.
   */
  VALUE   rb_str_substr(VALUE self, size_t starting_index, size_t length);

  /** Return a new String containing given number of copies of self. */
  VALUE   rb_str_times(VALUE self, VALUE times);

  /** Return an Integer obtained from String#to_i, using the given base. */
  VALUE   rb_str2inum(VALUE self, int base);

  /** Try to return a String using #to_str. Error raised if no or invalid conversion. */
  VALUE   rb_str_to_str(VALUE object);

  /** Call #to_s on object pointed to and _replace_ it with the String. */
  VALUE   rb_string_value(volatile VALUE* object_variable);

  char*   rb_string_value_ptr(volatile VALUE* object_variable);
  /**
   *  As rb_string_value but also returns a C string of the new String.
   */
  char*   rb_string_value_cstr(volatile VALUE* object_variable);

  /**
   * Returns an editable pointer to the String, the length is returned
   * in len parameter, which can be NULL.
   */
  char*   rb_str2cstr(VALUE string, long *len);

  long    rb_str_hash(VALUE str);

  void    rb_str_update(VALUE str, long beg, long end, VALUE replacement);

  VALUE rb_sprintf(const char* format, ...);

  VALUE   rb_str_equal(VALUE self, VALUE other);

  VALUE   rb_str_length(VALUE self);

  /** Raises an exception from the value of errno. */
  NORETURN(void rb_sys_fail(const char* mesg));

  /** Raises an exception from the value of the given errno. */
  NORETURN(void rb_syserr_fail(int err, const char* mesg));

  /** Evaluate the given string. */
  VALUE   rb_eval_string(const char* string);

  /** Create a String from the C string. */
  VALUE   rb_tainted_str_new2(const char* string);

  /** Create a String from the C string. */
  VALUE   rb_tainted_str_new(const char* string, long size);

  /** Issue a thread.pass. */
  void    rb_thread_schedule();

  /** Stubbed to always return 0. */
  int    rb_thread_alone();

  /** Request status of file descriptors */
  int     rb_thread_select(int max, fd_set* read, fd_set* write, fd_set* except,
                           struct timeval *timeval);

  /** Get current thread */
  VALUE   rb_thread_current(void);

  /** Returns a thread-local value. */
  VALUE rb_thread_local_aref(VALUE thread, ID id);

  /** Sets a thread-local value. */
  VALUE rb_thread_local_aset(VALUE thread, ID id, VALUE value);

  VALUE rb_thread_wakeup(VALUE thread);

  /** Marks the call as GC independent.
   *
   * Seriously, crazy restriction here. While the return value is the
   * value returned by func, it MUST not be a VALUE for a reference
   * object, since that means that reference objects were mutated
   * in an unmanaged way.
   *
   * The ubf function is not supported at all, so it can be anything.
   */
  typedef VALUE rb_blocking_function_t(void *);
  typedef void rb_unblock_function_t(void *);
  VALUE rb_thread_blocking_region(rb_blocking_function_t* func, void* data,
                                  rb_unblock_function_t* ubf, void* ubf_data);

  /* Experimental API. Call +func+ while being GC dependent. */
  typedef void* (*rb_thread_call_func)(void*);

  void* rb_thread_call_with_gvl(void* (*func)(void*), void* data);

  // Exists only to make extensions happy. It can be read and written to, but
  // it controls nothing.
  extern int rb_thread_critical;

  // These are used to allow for writing to ruby_verbose and ruby_debug. These
  // variables can be read, but writes ignored.
  extern int* mri_global_debug();
  extern int* mri_global_verbose();

  /** Get the current value of the record separator. @internal. */
  VALUE   mri_global_rb_rs();

  /** Get the value of the default record separator. @internal. */
  VALUE   mri_global_rb_default_rs();

  /** Get the value of the output record separator */
  VALUE   mri_global_rb_output_rs();

  /** Get the value of the output field separator */
  VALUE   mri_global_rb_output_fs();

  void    rb_lastline_set(VALUE obj);

  VALUE   rb_lastline_get(void);

#define HAVE_RB_THREAD_BLOCKING_REGION 1

  /* 1.9 provides these, so we will too: */
#define RUBY_UBF_IO ((rb_unblock_function_t *)-1)
#define RUBY_UBF_PROCESS ((rb_unblock_function_t *)-1)

  /** Return a new time object based on the given offset from the epoch */
  VALUE   rb_time_new(time_t sec, long usec);
  VALUE   rb_time_nano_new(time_t sec, long nsec);

  /** Returns an integer value representing the object's type. */
  int     rb_type(VALUE object);

  /** Call #to_sym on object. */
  ID      rb_to_id(VALUE object);

  /** Converts an object to an Integer by calling #to_int. */
  VALUE   rb_to_int(VALUE object);

  /** Module#undefine_method. Objects of class will not respond to name. @see rb_remove_method */
  void    rb_undef_method(VALUE module, const char* name);
  void    rb_undef(VALUE handle, ID name);

  /** Call block with given argument or raise error if no block given. */
  VALUE   rb_yield(VALUE argument);
  VALUE   rb_yield_values(int n, ...);
  VALUE   rb_yield_splat(VALUE array);

  VALUE   rb_apply(VALUE recv, ID mid, VALUE args);

  VALUE   rb_marshal_load(VALUE string);

  VALUE   rb_float_new(double val);
#define DBL2NUM(dbl) rb_float_new(dbl)

  VALUE   rb_Float(VALUE object);

  VALUE   rb_Integer(VALUE object);

  VALUE   rb_Rational(VALUE num, VALUE den);
#define rb_Rational1(x)   rb_Rational(x, INT2FIX(1))
#define rb_Rational2(x,y) rb_Rational(x, y)

  VALUE   rb_rational_new(VALUE, VALUE);
#define rb_rational_new1(x) rb_rational_new((x), INT2FIX(1))
#define rb_rational_new2(x,y) rb_rational_new((x), (y))

  VALUE   rb_Complex(VALUE real, VALUE imag);
#define rb_Complex1(x)   rb_Complex(x, INT2FIX(0))
#define rb_Complex2(x,y) rb_Complex(x, y)

  VALUE   rb_complex_new(VALUE, VALUE);
#define rb_complex_new1(x) rb_complex_new((x), INT2FIX(1))
#define rb_complex_new2(x,y) rb_complex_new((x), (y))

  NORETURN(void rb_num_zerodiv(void));

  NORETURN(void rb_bug(const char *fmt, ...));

  NORETURN(void rb_fatal(const char *fmt, ...));

  NORETURN(void rb_notimplement());

  NORETURN(VALUE rb_f_notimplement(int argc, VALUE *argv, VALUE obj));

  /** Raises an ArgumentError exception. */
  NORETURN(void rb_invalid_str(const char *str, const char *type));

  /** Print a warning if $VERBOSE is not nil. */
  void    rb_warn(const char *fmt, ...);

  /** Print a warning if $VERBOSE is true. */
  void    rb_warning(const char *fmt, ...);

  /** Creates a Range object from begin to end */
  VALUE   rb_range_new(VALUE begin, VALUE end, int exclude_end);

  VALUE   rb_range_beg_len(VALUE range, long* begp, long* lenp, long len, int err);

  /** Creates Range values and stores begin and end pointer */
  int rb_range_values(VALUE range, VALUE *begp, VALUE *endp, int *exclp);

  /** Callback to run when shutting down */
  void rb_set_end_proc(void* cb, VALUE cb_data);

#define RE_OPTION_IGNORECASE 1
#define RE_OPTION_EXTENDED   2
#define RE_OPTION_MULTILINE  4

  /** Creates a Regexp object */
  VALUE   rb_reg_new(const char *source, long len, int options);

  /** Retrieve the nth match for the given MatchData */
  VALUE   rb_reg_nth_match(long nth, VALUE match_data);

#define rb_thread_create(func, arg) capi_thread_create(func, arg, #func, __FILE__)
  /** Lock functions used by Onigmo */
  void    capi_reg_lock();
  void    capi_reg_unlock();

  /** New Enumerator. */
  VALUE   rb_enumeratorize(VALUE obj, VALUE meth, int argc, VALUE *argv);

#define RETURN_ENUMERATOR(obj, argc, argv) do {                 \
  if(!rb_block_given_p())                                       \
    return rb_enumeratorize((obj), ID2SYM(rb_frame_this_func()),\
                            (argc), (argv));                    \
} while (0)

  // include an extconf.h if one is provided
#ifdef RUBY_EXTCONF_H
#include RUBY_EXTCONF_H
#endif

#ifdef __cplusplus
}
#endif

#endif
