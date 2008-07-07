#ifndef __RUBY_H
#define __RUBY_H

#define RUBY

#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stddef.h>

/* Pointers are seen as totally opaque */
#define VALUE uintptr_t
#define ID uintptr_t

#ifdef __cplusplus
#define ANYARGS ...
#else
#define ANYARGS
#endif

/* We need to redefine those to casts to VALUE */
#undef Qfalse
#undef Qtrue
#undef Qnil
#undef Qundef

#define Qfalse ((VALUE)6L)
#define Qtrue  ((VALUE)10L)
#define Qnil   ((VALUE)14L)
#define Qundef ((VALUE)18L)

#undef RTEST
#undef NIL_P
#define RTEST(v) (((uintptr_t)(v) & 0x7) != 0x6)
#define NIL_P(v) (v == Qnil)
#define SYM2ID(sym) ((ID)(sym))
#define ID2SYM(id) ((VALUE)(id))

#undef ALLOC_N
#define ALLOC_N(kind, many) (kind*)malloc(sizeof(kind) * many)
#undef ALLOC
#define ALLOC(type) (type*)malloc(sizeof(type))
#define MEMZERO(p,type,n) memset((p), 0, sizeof(type)*(n))

#ifndef SYMBOL_P
int SYMBOL_P(VALUE obj);
#endif

extern VALUE rb_funcall(VALUE, ID, int cnt, ...);
extern VALUE rb_funcall2(VALUE, ID, int cnt, VALUE*);
int rb_scan_args(int, const VALUE*, const char*, ...);


extern VALUE subtend_get_global(int which);
extern VALUE subtend_get_exception(int which);

void rb_global_variable(VALUE* address);

void rb_define_method_(const char *file, VALUE vmod, const char *name, void *func, int args, int kind);
void rb_define_module_function(VALUE vmod, const char *name, void *func, int args);

void rb_include_module(VALUE parent, VALUE module);

#define rb_define_method(a, b, c, d) rb_define_method_(__FILE__, a, b, (void*)(c), d, 0)
#define rb_define_private_method(a, b, c, d) rb_define_method_(__FILE__, a, b, (void*)(c), d, 1)
#define rb_define_protected_method(a, b, c, d) rb_define_method_(__FILE__, a, b, (void*)(c), d, 2)
#define rb_define_singleton_method(a, b, c, d) rb_define_method_(__FILE__, a, b, (void*)(c), d, 3)
void rb_undef_method(VALUE klass, const char *name);
void rb_define_alloc_func(VALUE klass, void *func);

VALUE rb_define_class(const char *name, VALUE super);
VALUE rb_define_class_under(VALUE parent, const char *name, VALUE super);

VALUE rb_define_module(const char *name);
VALUE rb_define_module_under(VALUE parent, const char *name);

VALUE rb_call_super(int nargs, VALUE *args);

VALUE rb_const_get(VALUE klass, ID id);
void rb_define_const(VALUE klass, const char* key, VALUE val);
int rb_const_defined(VALUE klass, ID id);

VALUE rb_ivar_get(VALUE obj, ID sym);
VALUE rb_iv_get(VALUE obj, char *name);
VALUE rb_ivar_set(VALUE obj, ID sym, VALUE val);
VALUE rb_iv_set(VALUE obj, char *name, VALUE val);
void rb_define_attr(VALUE klass, const char* id, int read, int write);
VALUE rb_attr_get(VALUE obj, ID sym);

VALUE rb_cvar_defined(VALUE klass, ID id);

void rb_define_alias(VALUE obj, const char *new_name, const char *old_name);

int rb_block_given_p();
VALUE rb_each(VALUE obj);

enum {
  T_NONE,
  T_NIL,
  T_OBJECT,
  T_CLASS,
  T_ICLASS,
  T_MODULE,
  T_FLOAT,
  T_STRING,
  T_REGEXP,
  T_ARRAY,
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
  T_NODE,
};

int rb_type(VALUE obj);
#define TYPE(o) rb_type((VALUE)o)

/* If you change these numbers, please also modify subtend_get_global
 * in ruby.c.  It is safe to do so, these numbers have no inherent
 * meanings.
 */
#define rb_cObject        (subtend_get_global(0))
#define rb_cArray         (subtend_get_global(1))
#define rb_cBignum        (subtend_get_global(2))
#define rb_cClass         (subtend_get_global(3))
#define rb_cData          (subtend_get_global(4))
#define rb_cFalseClass    (subtend_get_global(5))
#define rb_cFixnum        (subtend_get_global(6))
#define rb_cFloat         (subtend_get_global(7))
#define rb_cHash          (subtend_get_global(8))
#define rb_cIO            (subtend_get_global(9))
#define rb_cModule        (subtend_get_global(10))
#define rb_cNilClass      (subtend_get_global(11))
#define rb_cRegexp        (subtend_get_global(12))
#define rb_cString        (subtend_get_global(13))
#define rb_cSymbol        (subtend_get_global(14))
#define rb_cThread        (subtend_get_global(15))
#define rb_cTrueClass     (subtend_get_global(16))
#define rb_cInteger       (subtend_get_global(17))

#define rb_mKernel rb_const_get(rb_cObject, rb_intern("Kernel") )
#define rb_mComparable rb_const_get(rb_cObject, rb_intern("Comparable") )
#define rb_mEnumerable rb_const_get(rb_cObject, rb_intern("Enumerable") )


/* TODO: Pull these into an enum */
#define rb_eException          subtend_get_exception(0)
#define rb_eSystemExit         subtend_get_exception(1)
#define rb_eInterrupt          subtend_get_exception(2)
#define rb_eSignal             subtend_get_exception(3)
#define rb_eFatal              subtend_get_exception(4) /* WTF is this? */
#define rb_eStandardError      subtend_get_exception(5)
#define rb_eRuntimeError       subtend_get_exception(6)
#define rb_eTypeError          subtend_get_exception(7)
#define rb_eArgError           subtend_get_exception(8)
#define rb_eIndexError         subtend_get_exception(9)
#define rb_eRangeError         subtend_get_exception(10)
#define rb_eNameError          subtend_get_exception(11)
#define rb_eNoMethodError      subtend_get_exception(12)
#define rb_eSecurityError      subtend_get_exception(13)
#define rb_eNotImpError        subtend_get_exception(14)
#define rb_eNoMemError         subtend_get_exception(15)

#define rb_eScriptError        subtend_get_exception(16)
#define rb_eSyntaxError        subtend_get_exception(17)
#define rb_eLoadError          subtend_get_exception(18)

#define rb_eSystemCallError    subtend_get_exception(19)
#define rb_mErrno              subtend_get_exception(20)
#define rb_eIOError            subtend_get_exception(21)
#define rb_eLocalJumpError     subtend_get_exception(22)

#define rb_eEOFError           subtend_get_exception(23)
#define rb_eThreadError        subtend_get_exception(24)
#define rb_eZeroDivError       subtend_get_exception(25)
#define rb_eFloatDomainError   subtend_get_exception(26)
#define rb_eSysStackError      subtend_get_exception(27)
#define rb_eRegexpError        subtend_get_exception(28)

void rb_raise(VALUE exc, const char *fmt, ...);

VALUE rb_require(const char *name);

VALUE rb_exc_new(VALUE klass, const char *str, long len);
VALUE rb_exc_new2(VALUE klass, const char *str);
VALUE rb_exc_new3(VALUE klass, VALUE str);

VALUE rb_obj_alloc(VALUE klass);
void rb_obj_call_init(VALUE obj, int nargs, VALUE *args);
VALUE rb_obj_is_instance_of(VALUE obj, VALUE klass);
VALUE rb_obj_is_kind_of(VALUE obj, VALUE klass);
int rb_respond_to(VALUE obj, ID sym);
ID rb_to_id(VALUE obj);

VALUE rb_yield(VALUE val);

int rb_safe_level();
void rb_secure(int level);
void rb_set_safe_level(int newlevel);

VALUE rb_gv_set(const char *name, VALUE value);
VALUE rb_gv_get(const char *name);

VALUE rb_check_array_type(VALUE ary);
VALUE rb_check_string_type(VALUE str);
VALUE rb_check_convert_type(VALUE val, int type, const char* tname, const char* method);
VALUE rb_convert_type(VALUE val, int type, const char* tname, const char* method);

VALUE rb_class_new_instance(int nargs, VALUE *args, VALUE klass);
VALUE rb_class_name(VALUE klass);
VALUE rb_singleton_class(VALUE klass);

void rb_thread_schedule();
#define CHECK_INTS
void rb_secure(int);

/* Conversions */
int FIX2INT(VALUE val);
VALUE INT2NUM(int num);
VALUE UINT2NUM(unsigned int num);
#define INT2FIX(v) INT2NUM(v)
#define NUM2INT(val) FIX2INT(val)
#define LONG2NUM(v) INT2NUM((int)v)
#define NUM2LONG(v) ((long)FIX2INT(v))
#define LONG2FIX(v) INT2FIX((int)v)
#define FIX2LONG(v) ((long)FIX2INT(v))

/* Array */
VALUE rb_Array(VALUE val);
VALUE rb_ary_new(void);
VALUE rb_ary_new2(long length);
VALUE rb_ary_new4(long length, const VALUE *val);
int rb_ary_size(VALUE self);
VALUE rb_ary_push(VALUE array, VALUE val);
VALUE rb_ary_pop(VALUE array);
VALUE rb_ary_entry(VALUE array, int offset);
VALUE rb_ary_clear(VALUE array);
VALUE rb_ary_dup(VALUE array);
VALUE rb_ary_join(VALUE array1, VALUE array2);
VALUE rb_ary_reverse(VALUE array);
VALUE rb_ary_unshift(VALUE array, VALUE val);
VALUE rb_ary_shift(VALUE array);
void rb_ary_store(VALUE array, int offset, VALUE val);

struct RArray {
  VALUE *ptr;
  long len;
};
typedef struct RArray RArray;

RArray* RARRAY(VALUE obj);
#define RARRAY_PTR(a) (RARRAY(a)->ptr)
#define RARRAY_LEN(a) (RARRAY(a)->len)

/* String */
VALUE rb_str_new(const char *ptr, long len);
VALUE rb_str_new2(const char *ptr);
VALUE rb_str_dup(VALUE str);
VALUE rb_str_append(VALUE str, VALUE str2);
VALUE rb_str_cat(VALUE str, const char *ptr, long len);
VALUE rb_str_cat2(VALUE str, const char *ptr);
VALUE rb_str_plus(VALUE str1, VALUE str2);
VALUE rb_str_buf_cat(VALUE str, const char *ptr, long len);
VALUE rb_str_buf_cat2(VALUE str, const char *ptr);
VALUE rb_str_buf_new(long capa);
VALUE rb_str_buf_append(VALUE str, VALUE str2);
int rb_str_cmp(VALUE str1, VALUE str2);
VALUE rb_str_split(VALUE str, const char *sep);
VALUE rb_str2inum(VALUE str, int base);
VALUE rb_cstr2inum(const char *str, int base);
VALUE rb_str_substr(VALUE str, long beg, long len);
VALUE rb_tainted_str_new2(const char *ptr);
char *StringValuePtr(VALUE str);
char *rb_string_value_cstr(VALUE *str);
#define StringValueCStr(str) rb_string_value_cstr(&(str))
VALUE rb_obj_as_string(VALUE obj);
char rb_str_get_char(VALUE arg, int index);
VALUE rb_str_to_str(VALUE arg);
VALUE rb_str_concat(VALUE str1, VALUE str2);
VALUE rb_str_times(VALUE str, VALUE times);
VALUE rb_String(VALUE val);

void rb_global_object(VALUE obj);
void rb_free_global(VALUE obj);

#define MAKE_GLOBAL(val) rb_global_object(val)
#define FREE_GLOBAL(val) rb_free_global(val)
#define rb_gc_mark_maybe(val) MAKE_GLOBAL(val)

VALUE rb_string_value(VALUE *obj);
#define StringValue(v) rb_string_value(&v)
#define SafeStringValue StringValue

/* HACK ? */
#define STR2CSTR StringValuePtr

struct RString {
  char *ptr;
  long len;
};
typedef struct RString RString;

RString* RSTRING(VALUE obj);
#define RSTRING_PTR(s) (RSTRING(s)->ptr)
#define RSTRING_LEN(s) (RSTRING(s)->len)

VALUE rb_inspect(VALUE obj);

/* Hash */
struct RHash {
    struct st_table *tbl;
};
typedef struct RHash RHash;

RHash* RHASH(VALUE obj);
VALUE rb_hash_new(void);
VALUE rb_hash_aref(VALUE hash, VALUE key);
VALUE rb_hash_aset(VALUE hash, VALUE key, VALUE val);
VALUE rb_hash_delete(VALUE hash, VALUE key);

/* Float */
struct RFloat {
  double value;
};
typedef struct RFloat RFloat;

RFloat* RFLOAT(VALUE obj);
VALUE rb_float_new(double d);
VALUE rb_Float(VALUE val);

/* Integer */
VALUE rb_Integer(VALUE val);

const char *rb_id2name(ID sym);
ID rb_intern(const char *name);
const char *rb_class2name(VALUE klass);
VALUE rb_class_of(VALUE obj);
#define CLASS_OF rb_class_of
#define rb_obj_class(obj) rb_class_of(obj)

const char *rb_obj_classname(VALUE obj);

#define rb_obj_freeze(obj) obj

/* RSTRING Macro Replacement */

char* rb_str_get_char_ptr(VALUE arg);
void rb_str_flush_char_ptr(VALUE arg, char* ptr);
int rb_str_get_char_len(VALUE arg);

VALUE subtend_wrap_struct(VALUE klass, void *struct_value, void *mark_func, void *free_func);
void* subtend_get_struct(VALUE obj);

// TODO: Make this do Check_Type as well
#define Data_Get_Struct(obj, type, sval) do { sval = (type *)subtend_get_struct(obj); } while (0)
#define Data_Wrap_Struct(klass, mark, free, sval) subtend_wrap_struct(klass, sval, (void*)(mark), (void*)(free))
#define Data_Make_Struct(klass,type,mark,free,sval) (\
    sval = ALLOC(type),\
    memset(sval, 0, sizeof(type)),\
    Data_Wrap_Struct(klass,mark,free,sval)\
)
void rb_check_type(VALUE x, int t);
#define Check_Type(v,t) rb_check_type((VALUE)(v),t)

struct RData {
  void *data;
  void (*dmark)(void*);
  void (*dfree)(void*);
};
typedef struct RData RData;

RData* RDATA(VALUE obj);
#define DATA_PTR(v) (RDATA(v)->data)

VALUE rb_path2class(const char *path);

/* Right now, Rubinius doesn't do tainted objects */
#define OBJ_TAINTED(x) 0
#define OBJ_TAINT(x)
#define OBJ_INFECT(x,s)

VALUE rb_proc_new(VALUE (*func)(), VALUE val);

void rb_define_hooked_variable(const char *name, VALUE *var, VALUE (*getter)(), void (*setter)());
void rb_define_variable(const char *name, VALUE *var);
void rb_define_readonly_variable(const char *name, VALUE *var);
void rb_define_virtual_variable(const char *name, VALUE (*getter)(), void (*setter)());

#endif
