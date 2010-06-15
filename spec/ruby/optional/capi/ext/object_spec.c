#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_OBJ_TAINT
static VALUE object_spec_OBJ_TAINT(VALUE self, VALUE obj) {
  OBJ_TAINT(obj);
  return Qnil;
}
#endif

#ifdef HAVE_OBJ_TAINTED
static VALUE object_spec_OBJ_TAINTED(VALUE self, VALUE obj) {
  return OBJ_TAINTED(obj) ? Qtrue : Qfalse;
}
#endif

#ifdef HAVE_RB_ANY_TO_S
static VALUE object_spec_rb_any_to_s(VALUE self, VALUE obj) {
  return rb_any_to_s(obj);
}
#endif

#ifdef HAVE_RB_ATTR_GET
static VALUE so_attr_get(VALUE self, VALUE obj, VALUE attr) {
  return rb_attr_get(obj, SYM2ID(attr));
}
#endif

#ifdef HAVE_RB_CHECK_ARRAY_TYPE
static VALUE so_check_array_type(VALUE self, VALUE ary) {
  return rb_check_array_type(ary);
}
#endif

#ifdef HAVE_RB_CHECK_CONVERT_TYPE
static VALUE so_check_convert_type(VALUE self, VALUE obj) {
  return rb_check_convert_type(obj, T_ARRAY, "Array", "to_ary");
}
#endif

#ifdef HAVE_RB_CHECK_FROZEN
static VALUE object_spec_rb_check_frozen(VALUE self, VALUE obj) {
  rb_check_frozen(obj);
  return Qnil;
}
#endif

#ifdef HAVE_RB_CHECK_STRING_TYPE
static VALUE so_check_string_type(VALUE self, VALUE str) {
  return rb_check_string_type(str);
}
#endif

#ifdef HAVE_RB_CLASS_OF
static VALUE so_rbclassof(VALUE self, VALUE obj) {
  return rb_class_of(obj);
}
#endif

#ifdef HAVE_RB_CONVERT_TYPE
static VALUE so_convert_type(VALUE self, VALUE obj) {
  return rb_convert_type(obj, T_ARRAY, "Array", "to_ary");
}
#endif

#ifdef HAVE_RB_EXTEND_OBJECT
static VALUE object_spec_rb_extend_object(VALUE self, VALUE obj, VALUE mod) {
  rb_extend_object(obj, mod);
  return obj;
}
#endif

#ifdef HAVE_RB_INSPECT
static VALUE so_inspect(VALUE self, VALUE obj) {
  return rb_inspect(obj);
}
#endif

#ifdef HAVE_RB_OBJ_ALLOC
static VALUE so_rb_obj_alloc(VALUE self, VALUE klass) {
  return rb_obj_alloc(klass);
}
#endif

#ifdef HAVE_RB_OBJ_CALL_INIT
static VALUE so_rb_obj_call_init(VALUE self, VALUE object,
                                 VALUE nargs, VALUE args) {
  int c_nargs = FIX2INT(nargs);
  VALUE c_args[c_nargs];
  int i;

  for (i = 0; i < c_nargs; i++)
    c_args[i] = rb_ary_entry(args, i);

  rb_obj_call_init(object, c_nargs, c_args);

  return Qnil;
}
#endif

#ifdef HAVE_RB_OBJ_CLASSNAME
static VALUE so_rbobjclassname(VALUE self, VALUE obj) {
  return rb_str_new2(rb_obj_classname(obj));
}

#endif

#ifdef HAVE_RB_OBJ_FREEZE
static VALUE object_spec_rb_obj_freeze(VALUE self, VALUE obj) {
  return rb_obj_freeze(obj);
}
#endif

#ifdef HAVE_RB_OBJ_FROZEN_P
static VALUE object_spec_rb_obj_frozen_p(VALUE self, VALUE obj) {
  return rb_obj_frozen_p(obj);
}
#endif

#ifdef HAVE_RB_OBJ_ID
static VALUE object_spec_rb_obj_id(VALUE self, VALUE obj) {
  return rb_obj_id(obj);
}
#endif

#ifdef HAVE_RB_OBJ_IS_INSTANCE_OF
static VALUE so_instance_of(VALUE self, VALUE obj, VALUE klass) {
  return rb_obj_is_instance_of(obj, klass);
}
#endif

#ifdef HAVE_RB_OBJ_IS_KIND_OF
static VALUE so_kind_of(VALUE self, VALUE obj, VALUE klass) {
  return rb_obj_is_kind_of(obj, klass);
}
#endif

#ifdef HAVE_RB_OBJ_TAINT
static VALUE object_spec_rb_obj_taint(VALUE self, VALUE obj) {
  return rb_obj_taint(obj);
}
#endif

#ifdef HAVE_RB_REQUIRE
static VALUE so_require(VALUE self) {
  rb_require("fixtures/foo");
  return Qnil;
}
#endif

#ifdef HAVE_RB_RESPOND_TO
static VALUE so_respond_to(VALUE self, VALUE obj, VALUE sym) {
  return rb_respond_to(obj, SYM2ID(sym)) ? Qtrue : Qfalse;
}
#endif

#ifdef HAVE_RB_SPECIAL_CONST_P
static VALUE object_spec_rb_special_const_p(VALUE self, VALUE value) {
  return rb_special_const_p(value);
}
#endif

#ifdef HAVE_RB_TO_ID
static VALUE so_to_id(VALUE self, VALUE obj) {
  return ID2SYM(rb_to_id(obj));
}
#endif

#ifdef HAVE_RTEST
static VALUE object_spec_RTEST(VALUE self, VALUE value) {
  return RTEST(value) ? Qtrue : Qfalse;
}
#endif

#ifdef HAVE_TYPE
static VALUE so_is_type_nil(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_NIL) {
    return Qtrue;
  }
  return Qfalse;
}

static VALUE so_is_type_object(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_OBJECT) {
    return Qtrue;
  }
  return Qfalse;
}

static VALUE so_is_type_array(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_ARRAY) {
    return Qtrue;
  }
  return Qfalse;
}

static VALUE so_is_type_module(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_MODULE) {
    return Qtrue;
  }
  return Qfalse;
}

static VALUE so_is_type_class(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_CLASS) {
    return Qtrue;
  }
  return Qfalse;
}
#endif

#ifdef HAVE_RB_TO_INT
static VALUE object_spec_rb_to_int(VALUE self, VALUE obj) {
  return rb_to_int(obj);
}
#endif

void Init_object_spec() {
  VALUE cls;
  cls = rb_define_class("CApiObjectSpecs", rb_cObject);

#ifdef HAVE_OBJ_TAINT
  rb_define_method(cls, "OBJ_TAINT", object_spec_OBJ_TAINT, 1);
#endif

#ifdef HAVE_OBJ_TAINTED
  rb_define_method(cls, "OBJ_TAINTED", object_spec_OBJ_TAINTED, 1);
#endif

#ifdef HAVE_RB_ANY_TO_S
  rb_define_method(cls, "rb_any_to_s", object_spec_rb_any_to_s, 1);
#endif

#ifdef HAVE_RB_ATTR_GET
  rb_define_method(cls, "rb_attr_get", so_attr_get, 2);
#endif

#ifdef HAVE_RB_CHECK_ARRAY_TYPE
  rb_define_method(cls, "rb_check_array_type", so_check_array_type, 1);
#endif

#ifdef HAVE_RB_CHECK_CONVERT_TYPE
  rb_define_method(cls, "rb_check_convert_type", so_check_convert_type, 1);
#endif

#ifdef HAVE_RB_CHECK_FROZEN
  rb_define_method(cls, "rb_check_frozen", object_spec_rb_check_frozen, 1);
#endif

#ifdef HAVE_RB_CHECK_STRING_TYPE
  rb_define_method(cls, "rb_check_string_type", so_check_string_type, 1);
#endif

#ifdef HAVE_RB_CLASS_OF
  rb_define_method(cls, "rb_class_of", so_rbclassof, 1);
#endif

#ifdef HAVE_RB_CONVERT_TYPE
  rb_define_method(cls, "rb_convert_type", so_convert_type, 1);
#endif

#ifdef HAVE_RB_EXTEND_OBJECT
  rb_define_method(cls, "rb_extend_object", object_spec_rb_extend_object, 2);
#endif

#ifdef HAVE_RB_INSPECT
  rb_define_method(cls, "rb_inspect", so_inspect, 1);
#endif

#ifdef HAVE_RB_OBJ_ALLOC
  rb_define_method(cls, "rb_obj_alloc", so_rb_obj_alloc, 1);
#endif

#ifdef HAVE_RB_OBJ_CALL_INIT
  rb_define_method(cls, "rb_obj_call_init", so_rb_obj_call_init, 3);
#endif

#ifdef HAVE_RB_OBJ_CLASSNAME
  rb_define_method(cls, "rb_obj_classname", so_rbobjclassname, 1);
#endif

#ifdef HAVE_RB_OBJ_FREEZE
  rb_define_method(cls, "rb_obj_freeze", object_spec_rb_obj_freeze, 1);
#endif

#ifdef HAVE_RB_OBJ_FROZEN_P
  rb_define_method(cls, "rb_obj_frozen_p", object_spec_rb_obj_frozen_p, 1);
#endif

#ifdef HAVE_RB_OBJ_ID
  rb_define_method(cls, "rb_obj_id", object_spec_rb_obj_id, 1);
#endif

#ifdef HAVE_RB_OBJ_IS_INSTANCE_OF
  rb_define_method(cls, "rb_obj_is_instance_of", so_instance_of, 2);
#endif

#ifdef HAVE_RB_OBJ_IS_KIND_OF
  rb_define_method(cls, "rb_obj_is_kind_of", so_kind_of, 2);
#endif

#ifdef HAVE_RB_OBJ_TAINT
  rb_define_method(cls, "rb_obj_taint", object_spec_rb_obj_taint, 1);
#endif

#ifdef HAVE_RB_REQUIRE
  rb_define_method(cls, "rb_require", so_require, 0);
#endif

#ifdef HAVE_RB_RESPOND_TO
  rb_define_method(cls, "rb_respond_to", so_respond_to, 2);
#endif

#ifdef HAVE_RB_SPECIAL_CONST_P
  rb_define_method(cls, "rb_special_const_p", object_spec_rb_special_const_p, 1);
#endif

#ifdef HAVE_RB_STR_NEW2
#endif

#ifdef HAVE_RB_TO_ID
  rb_define_method(cls, "rb_to_id", so_to_id, 1);
#endif

#ifdef HAVE_RTEST
  rb_define_method(cls, "RTEST", object_spec_RTEST, 1);
#endif

#ifdef HAVE_TYPE
  rb_define_method(cls, "rb_is_type_nil", so_is_type_nil, 1);
  rb_define_method(cls, "rb_is_type_object", so_is_type_object, 1);
  rb_define_method(cls, "rb_is_type_array", so_is_type_array, 1);
  rb_define_method(cls, "rb_is_type_module", so_is_type_module, 1);
  rb_define_method(cls, "rb_is_type_class", so_is_type_class, 1);
#endif

#ifdef HAVE_RB_TO_INT
  rb_define_method(cls, "rb_to_int", object_spec_rb_to_int, 1);
#endif
}

#ifdef __cplusplus
}
#endif
