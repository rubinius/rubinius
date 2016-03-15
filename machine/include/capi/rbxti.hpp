#ifndef RUBINIUS_RBXTI_HPP
#define RUBINIUS_RBXTI_HPP

#include <stdint.h>

namespace rbxti {

  class InternalObject {};
  class InternalSymbol : public InternalObject {};
  class InternalTable : public InternalObject {};
  class InternalCompiledCode : public InternalObject {};
  class InternalMachineCode : public InternalObject {};
  class InternalModule : public InternalObject {};
  class InternalString : public InternalObject {};
  class InternalInteger : public InternalObject {};
  class InternalArray : public InternalObject {};


  typedef InternalObject* robject;
  typedef InternalSymbol* rsymbol;
  typedef InternalTable* rtable;
  typedef InternalCompiledCode* rcompiled_code;
  typedef InternalMachineCode* rmachine_code;
  typedef InternalModule* rmodule;
  typedef InternalString* rstring;
  typedef InternalInteger* rinteger;
  typedef InternalArray* rarray;

  typedef int64_t r_mint;

  class Env;

  class EnvPrivate;

  const static int GCYoung = 1;
  const static int GCMature = 2;
  const static int GCFinalizer = 4;

  typedef robject (*results_func)(Env* env);
  typedef void  (*enable_func)(Env* env);
  typedef void* (*enter_method)(Env* env, robject recv, rsymbol name,
                                rmodule mod, rcompiled_code code);
  typedef void* (*enter_block)(Env* env, rsymbol name,
                               rmodule module, rcompiled_code code);
  typedef void  (*leave_func)(Env* env, void* tag);
  typedef void* (*enter_gc)(Env* env, int level);
  typedef void* (*enter_script)(Env* env, rcompiled_code code);
  typedef void  (*shutdown_func)(Env* env);

  typedef void  (*thread_start_func)(Env* env);
  typedef void  (*thread_stop_func)(Env* env);

  typedef void  (*at_gc_func)(Env* env);

  typedef void  (*at_ip_func)(Env* env, rmachine_code code, int ip);

  typedef void  (*compiled_code_iterator)(Env* env, rcompiled_code code, void* data);

  class Env {
  public:
    EnvPrivate* private_;

    long config_get_int(const char* name);
    void config_set(const char* name, const char* val);

    ////

    rsymbol  cast_to_rsymbol  (robject obj);
    rtable   cast_to_rtable   (robject obj);
    rcompiled_code  cast_to_rcompiled_code  (robject obj);
    rmodule  cast_to_rmodule  (robject obj);
    rstring  cast_to_rstring  (robject obj);
    rinteger cast_to_rinteger (robject obj);
    rarray   cast_to_rarray   (robject obj);

    void* global_tool_data();
    void set_global_tool_data(void* d);

    int thread_tool_new_id();
    void* thread_tool_data(int id);
    void thread_tool_set_data(int id, void* data);

    int current_thread_id();

    void enable_thread_tooling();
    void disable_thread_tooling();

    rinteger integer_new(r_mint val);
    r_mint integer_value(rinteger i);

    rsymbol symbol(const char* data);
    void symbol_cstr(rsymbol sym, char* data, int size);
    rstring symbol_to_string(rsymbol sym);
    r_mint symbol_id(rsymbol sym);

    rstring string_new(const char* data);
    rstring to_s(robject obj);
    rsymbol string_to_symbol(rstring str);

    rsymbol module_name(rmodule mod);
    bool module_is_metaclass(rmodule mod);
    robject metaclass_attached_instance(rmodule mod);

    bool is_nil(robject obj);
    robject nil();

    rsymbol method_file(rcompiled_code code);
    r_mint method_line(rcompiled_code code);
    r_mint method_id(rcompiled_code code);

    void find_all_compiled_code(compiled_code_iterator func, void* data);

    r_mint machine_code_id(rmachine_code code);

    rtable table_new();
    robject table_fetch(rtable tbl, robject key, bool* fetched);
    void table_store(rtable tbl, robject key, robject val);

    rarray array_new(int size);
    void array_set(rarray ary, int idx, robject obj);
    robject array_get(rarray ary, int idx);

    uint64_t time_current_ns();

    ////
    void set_tool_enter_method(enter_method func);
    void set_tool_leave_method(leave_func func);

    void set_tool_enter_block(enter_block func);
    void set_tool_leave_block(leave_func func);

    void set_tool_enter_gc(enter_gc func);
    void set_tool_leave_gc(leave_func func);

    void set_tool_enter_script(enter_script func);
    void set_tool_leave_script(leave_func func);

    void set_tool_results(results_func func);
    void set_tool_enable(enable_func func);

    void set_tool_shutdown(shutdown_func func);

    void set_tool_thread_start(thread_start_func func);
    void set_tool_thread_stop(thread_stop_func func);

    void set_tool_at_gc(at_gc_func func);

    void set_tool_at_ip(at_ip_func func);
  };
}

#endif
