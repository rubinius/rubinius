#include <ruby.h>

VALUE sc_get_object (VALUE self) { 
  return rb_cObject;
}

VALUE sc_get_kernel (VALUE self) { 
  return rb_mKernel; 
}

VALUE sc_get_exception (VALUE self) {
  return rb_eException;
}

VALUE sc_get_system_exit (VALUE self) {
  return rb_eSystemExit;
}

VALUE sc_get_interrupt (VALUE self) {
  return rb_eInterrupt;
}

VALUE sc_get_signal (VALUE self) {
  return rb_eSignal;
}

VALUE sc_get_standard_error (VALUE self) {
  return rb_eStandardError;
}

VALUE sc_get_runtime_error (VALUE self) {
  return rb_eRuntimeError;
}

VALUE sc_get_type_error (VALUE self) {
  return rb_eTypeError;
}

VALUE sc_get_arg_error (VALUE self) {
  return rb_eArgError;
}

VALUE sc_get_index_error (VALUE self) {
  return rb_eIndexError;
}

VALUE sc_get_range_error (VALUE self) {
  return rb_eRangeError;
}

VALUE sc_get_name_error (VALUE self) {
  return rb_eNameError;
}

VALUE sc_get_no_method_error (VALUE self) {
  return rb_eNoMethodError;
}

VALUE sc_get_security_error (VALUE self) {
  return rb_eSecurityError;
}

VALUE sc_get_not_imp_error (VALUE self) {
  return rb_eNotImpError;
}

VALUE sc_get_no_mem_error (VALUE self) {
  return rb_eNoMemError;
}

VALUE sc_get_script_error (VALUE self) {
  return rb_eScriptError;
}

VALUE sc_get_syntax_error (VALUE self) {
  return rb_eSyntaxError;
}

VALUE sc_get_load_error (VALUE self) {
  return rb_eLoadError;
}

VALUE sc_get_system_call_error (VALUE self) {
  return rb_eSystemCallError;
}

VALUE sc_get_errno (VALUE self) {
  return rb_mErrno;
}

VALUE sc_get_io_error (VALUE self) {
  return rb_eIOError;
}

VALUE sc_get_local_jump_error (VALUE self) {
  return rb_eLocalJumpError;
}

VALUE sc_get_eof_error (VALUE self) {
  return rb_eEOFError;
}

VALUE sc_get_thread_error (VALUE self) {
  return rb_eThreadError;
}

VALUE sc_get_zero_division_error (VALUE self) {
  return rb_eZeroDivError;
}

VALUE sc_get_float_domain_error (VALUE self) {
  return rb_eFloatDomainError;
}

VALUE sc_get_sys_stack_error (VALUE self) {
  return rb_eSysStackError;
}

VALUE sc_get_regexp_error (VALUE self) {
  return rb_eRegexpError;
}

void Init_constants_spec() {
  VALUE cls;
  cls = rb_define_class("CApiConstantsSpecs", rb_cObject);
  rb_define_method(cls, "rb_object", sc_get_object, 0);
  rb_define_method(cls, "rb_kernel", sc_get_kernel, 0);
  rb_define_method(cls, "rb_exception", sc_get_exception, 0);
  rb_define_method(cls, "rb_system_exit", sc_get_system_exit, 0);
  rb_define_method(cls, "rb_interrupt", sc_get_interrupt, 0);
  rb_define_method(cls, "rb_signal", sc_get_signal, 0);
  rb_define_method(cls, "rb_standard_error", sc_get_standard_error, 0);
  rb_define_method(cls, "rb_runtime_error", sc_get_runtime_error, 0);
  rb_define_method(cls, "rb_type_error", sc_get_type_error, 0);
  rb_define_method(cls, "rb_arg_error", sc_get_arg_error, 0);
  rb_define_method(cls, "rb_index_error", sc_get_index_error, 0);
  rb_define_method(cls, "rb_range_error", sc_get_range_error, 0);
  rb_define_method(cls, "rb_name_error", sc_get_name_error, 0);
  rb_define_method(cls, "rb_no_method_error", sc_get_no_method_error, 0);
  rb_define_method(cls, "rb_security_error", sc_get_security_error, 0);
  rb_define_method(cls, "rb_not_imp_error", sc_get_not_imp_error, 0);
  rb_define_method(cls, "rb_no_mem_error", sc_get_no_mem_error, 0);
  rb_define_method(cls, "rb_script_error", sc_get_script_error, 0);
  rb_define_method(cls, "rb_syntax_error", sc_get_syntax_error, 0);
  rb_define_method(cls, "rb_load_error", sc_get_load_error, 0);
  rb_define_method(cls, "rb_system_call_error", sc_get_system_call_error, 0);
  rb_define_method(cls, "rb_errno", sc_get_errno, 0);
  rb_define_method(cls, "rb_io_error", sc_get_io_error, 0);
  rb_define_method(cls, "rb_local_jump_error", sc_get_local_jump_error, 0);
  rb_define_method(cls, "rb_eof_error", sc_get_eof_error, 0);
  rb_define_method(cls, "rb_thread_error", sc_get_thread_error, 0);
  rb_define_method(cls, "rb_zero_division_error", sc_get_zero_division_error, 0);
  rb_define_method(cls, "rb_float_domain_error", sc_get_float_domain_error, 0);
  rb_define_method(cls, "rb_sys_stack_error", sc_get_sys_stack_error, 0);
  rb_define_method(cls, "rb_regexp_error", sc_get_regexp_error, 0);
}

