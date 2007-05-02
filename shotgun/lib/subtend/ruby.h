

/* Pointers are seen as totally opaque */
#define VALUE void*
#define ID int

#ifndef Qfalse

#define Qfalse ((VALUE)0L)
#define Qtrue  ((VALUE)2L)
#define Qnil   ((VALUE)4L)
#define Qundef ((VALUE)6L)

#endif

#define SYM2ID(sym) ((ID)(sym))

extern VALUE rb_funcall(VALUE, ID, int cnt, ...);
extern VALUE rb_funcall2(VALUE, ID, int cnt, VALUE*);

extern VALUE subtend_get_global(int which);

void rb_define_method_(char *file, VALUE vmod, char *name, void *func, int args, int kind);

#define rb_define_method(a, b, c, d) rb_define_method_(__FILE__, a, b, c, d, 0)
#define rb_define_singleton_method(a, b, c, d) rb_define_method_(__FILE__, a, b, c, d, 1)

VALUE rb_define_class(char *name, VALUE super);

#define rb_cObject (subtend_get_global(0))

VALUE rb_ary_new(void);
VALUE rb_ary_new2(long length);
VALUE rb_ary_get(VALUE self, int index);
VALUE rb_ary_set(VALUE self, int index, VALUE val);
int rb_ary_size(VALUE self);
VALUE rb_ary_push(VALUE array, VALUE val);
VALUE rb_str_new(const char *ptr, long len);
VALUE rb_str_dup(VALUE str);
VALUE rb_str_buf_cat(VALUE str, const char *ptr, long len);

char *rb_id2name(ID sym);

