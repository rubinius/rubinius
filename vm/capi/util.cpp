#include "config.h"
#include "windows_compat.h"

#include "call_frame.hpp"
#include "builtin/string.hpp"
#include "capi/capi.hpp"
#include "capi/ruby.h"

#include <stdarg.h>

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  int rb_scan_args_18(int argc, const VALUE* argv, const char* spec, ...) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    int n, i = 0;
    const char *p = spec;
    VALUE *var;
    va_list vargs;

    va_start(vargs, spec);

    if(*p == '*') goto rest_arg;

    if(ISDIGIT(*p)) {
      n = *p - '0';
      if(n > argc)
        rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)", argc, n);
      for(i=0; i<n; i++) {
        var = va_arg(vargs, VALUE*);
        if(var) *var = argv[i];
      }
      p++;
    } else {
      goto error;
    }

    if(ISDIGIT(*p)) {
      n = i + *p - '0';
      for(; i<n; i++) {
        var = va_arg(vargs, VALUE*);
        if(argc > i) {
          if(var) *var = argv[i];
        } else {
          if(var) *var = Qnil;
        }
      }
      p++;
    }

    if(*p == '*') {
      rest_arg:
      var = va_arg(vargs, VALUE*);
      if(argc > i) {
        if(var) *var = rb_ary_new4(argc-i, argv+i);
        i = argc;
      } else {
        if(var) *var = rb_ary_new();
      }
      p++;
    }

    if(*p == '&') {
      var = va_arg(vargs, VALUE*);
      *var = env->get_handle(env->block());
      p++;
    }
    va_end(vargs);

    if(*p != '\0') {
      goto error;
    }

    if(argc > i) {
      rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)", argc, i);
    }

    return argc;

    error:
    rb_raise(rb_eFatal, "bad scan arg format: %s", spec);
    return 0;
  }

  int rb_scan_args_19(int argc, const VALUE *argv, const char *fmt, ...) {
    int i;
    const char *p = fmt;
    VALUE *var;
    va_list vargs;
    int f_var = 0, f_hash = 0, f_block = 0;
    int n_lead = 0, n_opt = 0, n_trail = 0, n_mand;
    int argi = 0;
    VALUE hash = Qnil;

    if(ISDIGIT(*p)) {
      n_lead = *p - '0';
      p++;

      if(ISDIGIT(*p)) {
        n_opt = *p - '0';
        p++;

        if(ISDIGIT(*p)) {
          n_trail = *p - '0';
          p++;
          goto block_arg;
        }
      }
    }

    if(*p == '*') {
      f_var = 1;
      p++;

      if(ISDIGIT(*p)) {
        n_trail = *p - '0';
        p++;
      }
    }

  block_arg:
    if(*p == ':') {
      f_hash = 1;
      p++;
    }

    if(*p == '&') {
      f_block = 1;
      p++;
    }

    if(*p != '\0') {
      rb_fatal("bad scan arg format: %s", fmt);
    }
    n_mand = n_lead + n_trail;

    if(argc < n_mand) {
      goto argc_error;
    }

    va_start(vargs, fmt);

    /* capture an option hash - phase 1: pop */
    if(f_hash && n_mand < argc) {
      VALUE last = argv[argc - 1];

      if(NIL_P(last)) {
      /* nil is taken as an empty option hash only if it is not
       * ambiguous; i.e. '*' is not specified and arguments are
       * given more than sufficient
       */
      if(!f_var && n_mand + n_opt < argc)
        argc--;
      } else {
        hash = rb_check_convert_type(last, T_HASH, "Hash", "to_hash");
        if(!NIL_P(hash))
        argc--;
      }
    }

    /* capture leading mandatory arguments */
    for(i = n_lead; i-- > 0; ) {
      var = va_arg(vargs, VALUE *);
      if(var) *var = argv[argi];
      argi++;
    }

    /* capture optional arguments */
    for(i = n_opt; i-- > 0; ) {
      var = va_arg(vargs, VALUE *);
      if(argi < argc - n_trail) {
        if(var) *var = argv[argi];
        argi++;
      } else {
        if(var) *var = Qnil;
      }
    }

    /* capture variable length arguments */
    if(f_var) {
      int n_var = argc - argi - n_trail;

      var = va_arg(vargs, VALUE *);
      if(0 < n_var) {
        if(var) *var = rb_ary_new4(n_var, &argv[argi]);
        argi += n_var;
      } else {
        if(var) *var = rb_ary_new();
      }
    }

    /* capture trailing mandatory arguments */
    for(i = n_trail; i-- > 0; ) {
      var = va_arg(vargs, VALUE *);
      if(var) *var = argv[argi];
      argi++;
    }

    /* capture an option hash - phase 2: assignment */
    if(f_hash) {
      var = va_arg(vargs, VALUE *);
      if(var) *var = hash;
    }

    /* capture iterator block */
    if(f_block) {
      var = va_arg(vargs, VALUE *);
      if(rb_block_given_p()) {
        *var = rb_block_proc();
      } else {
        *var = Qnil;
      }
    }

    va_end(vargs);

    if(argi < argc) {
      goto argc_error;
    }

    return argc;

  argc_error:
    if(0 < n_opt) {
      rb_raise(rb_eArgError, "wrong number of arguments (%d for %d..%d%s)",
               argc, n_mand, n_mand + n_opt, f_var ? "+" : "");
    } else {
      rb_raise(rb_eArgError, "wrong number of arguments (%d for %d%s)",
               argc, n_mand, f_var ? "+" : "");
    }
  }

  int rb_safe_level() {
    return FIX2INT(rb_gv_get("$SAFE"));
  }

  /** Some arbitrary high max for $SAFE. */
  static const int RBX_ARBITRARY_MAX_SAFE_LEVEL = 256;

  /**
   *  @todo   Error out if level is invalid?
   *          Or should it fail silently as
   *          a security measure? --rue
   */
  void rb_set_safe_level(int new_level) {
    int safe_level = rb_safe_level();

    if(new_level > safe_level && new_level < RBX_ARBITRARY_MAX_SAFE_LEVEL) {
      rb_gv_set("$SAFE", INT2FIX(new_level));
    }
  }

  void rb_secure(int level) {
    if(level <= rb_safe_level())
    {
      rb_raise(rb_eSecurityError, "Insecure operation at level %d", rb_safe_level());
    }
  }

  int rb_special_const_p(VALUE obj) {
    return SPECIAL_CONST_P(obj) ? Qtrue : Qfalse;
  }

  void rb_invalid_str(const char *str, const char *type) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* string = String::create(env->state(), str);

    // MRI does this, probably to escape non-printing chars in the string
    VALUE s = rb_funcall(env->get_handle(string), rb_intern("inspect"), 0);
    string = c_as<String>(env->get_object(s));

    rb_raise(rb_eArgError, "invalid value for %s: %s", type, string->c_str(env->state()));
  }

  void ruby_setenv(const char *name, const char *value) {
    setenv(name, value, 1);
  }

  void rb_iter_break() {
    capi::capi_raise_break(Qnil);
    exit(1); // compiler snack
  }

  // Imported from MRI
  char *ruby_strdup(const char *str) {
    char *tmp;
    size_t len = strlen(str) + 1;

    tmp = (char*)xmalloc(len);
    memcpy(tmp, str, len);

    return tmp;
  }

#if SIZEOF_INT < SIZEOF_LONG
  int rb_long2int(long n) {
    int i = (int)n;
    if((long)i != n) rb_raise(rb_eRangeError, "long value is outside of range of int");
    return i;
  }
#endif

  const char* rb_sourcefile() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    CallFrame* call_frame = env->current_call_frame()->top_ruby_frame();
    if(call_frame->compiled_code) {
      Symbol* name = try_as<Symbol>(call_frame->compiled_code->file());
      if(name) {
        VALUE str = env->get_handle(name->to_str(env->state()));
        return rb_str_ptr_readonly(str);
      }
    }
    return NULL;
  }

  int rb_sourceline() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    CallFrame* call_frame = env->current_call_frame()->top_ruby_frame();
    return call_frame->line(env->state());
  }

}
