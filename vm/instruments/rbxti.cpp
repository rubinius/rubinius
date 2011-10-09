#include "instruments/timing.hpp"

#include "rbxti-internal.hpp"

#include "vm/vm.hpp"
#include "vmmethod.hpp"
#include "configuration.hpp"
#include "config_parser.hpp"

#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"
#include "builtin/integer.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "instruments/tooling.hpp"

#include "object_utils.hpp"

using namespace rubinius;

namespace rubinius {
  namespace tooling {
    class ToolBroker;
  }
}

namespace rbxti {
  class EnvPrivate {
    VM* state_;
    void* tool_data[rubinius::tooling::cTotalToolDatas];
    rubinius::tooling::ToolBroker* broker_;

  public:
    EnvPrivate(STATE)
      : state_(state)
      , broker_(state->shared.tool_broker())
    {
      for(int i = 0; i < rubinius::tooling::cTotalToolDatas; i++) {
        tool_data[i] = 0;
      }
    }

    VM* state() { return state_; }

    void* get_tool_data(int i) {
      return tool_data[i];
    }

    void set_tool_data(int i, void* data) {
      tool_data[i] = data;
    }

    rubinius::tooling::ToolBroker* global() {
      return broker_;
    }
  };

  long Env::config_get_int(const char* name) {
    config::ConfigItem* item = private_->state()->shared.config.find(name);
    if(config::Integer* cint = dynamic_cast<config::Integer*>(item)) {
      return cint->value;
    }
    return 0;
  }

  void Env::config_set(const char* name, const char* val) {
    if(private_->state()->shared.config.import(name, val)) return;

    private_->state()->shared.user_variables.set(name, val);
  }

  rsymbol Env::cast_to_rsymbol(robject obj) {
    if(Symbol* sym = try_as<Symbol>((Object*)obj)) {
      return o(sym);
    }

    return NULL;
  }

  rtable Env::cast_to_rtable(robject obj) {
    if(LookupTable* c = try_as<LookupTable>((Object*)obj)) {
      return o(c);
    }

    return NULL;
  }

  rmethod Env::cast_to_rmethod(robject obj) {
    if(CompiledMethod* c = try_as<CompiledMethod>((Object*)obj)) {
      return o(c);
    }

    return NULL;
  }

  rmodule Env::cast_to_rmodule(robject obj) {
    if(Module* c = try_as<Module>(s(obj))) {
      return o(c);
    }

    return NULL;
  }

  rstring Env::cast_to_rstring(robject obj) {
    if(String* c = try_as<String>(s(obj))) {
      return o(c);
    }

    return NULL;
  }

  rinteger Env::cast_to_rinteger(robject obj) {
    if(Integer* c = try_as<Integer>(s(obj))) {
      return o(c);
    }

    return NULL;
  }

  rarray Env::cast_to_rarray(robject obj) {
    if(Array* c = try_as<Array>(s(obj))) {
      return o(c);
    }

    return NULL;
  }

  void* Env::global_tool_data() {
    return private_->global()->global_tool_data();
  }

  void Env::set_global_tool_data(void* d) {
    private_->global()->set_global_tool_data(d);
  }

  int Env::thread_tool_new_id() {
    return private_->global()->allocate_tool_id();
  }

  void* Env::thread_tool_data(int id) {
    return private_->get_tool_data(id);
  }

  void Env::thread_tool_set_data(int id, void* data) {
    private_->set_tool_data(id, data);
  }

  void Env::enable_thread_tooling() {
    private_->state()->enable_tooling();
  }

  void Env::disable_thread_tooling() {
    private_->state()->disable_tooling();
  }

  int Env::current_thread_id() {
    return private_->state()->thread_id();
  }

  rinteger Env::integer_new(r_mint val) {
    return o(Integer::from(private_->state(), val));
  }

  r_mint Env::integer_value(rinteger ri) {
    return (r_mint)i(ri)->to_native();
  }

  rsymbol Env::symbol(const char* data) {
    return o(private_->state()->symbol(data));
  }

  void Env::symbol_cstr(rsymbol rsym, char* data, int size) {
    Symbol* sym = i(rsym);
    std::string cur = sym->debug_str(private_->state());

    int out = cur.size();
    if(out > size - 1) out = size - 1;

    memcpy(data, cur.c_str(), out);
    data[out] = 0;
  }

  rstring Env::symbol_to_string(rsymbol sym) {
    return o(i(sym)->to_str(private_->state()));
  }

  r_mint Env::symbol_id(rsymbol sym) {
    return i(sym)->index();
  }

  rstring Env::string_new(const char* ptr) {
    return o(String::create(private_->state(), ptr));
  }

  rstring Env::to_s(robject obj) {
    return o(((Object*)obj)->to_s(private_->state()));
  }

  rsymbol Env::string_to_symbol(rstring str) {
    return o(i(str)->to_sym(private_->state()));
  }

  bool Env::is_nil(robject obj) {
    return s(obj)->nil_p();
  }

  robject Env::nil() {
    return s(Qnil);
  }

  rsymbol Env::method_file(rmethod cm) {
    return o(i(cm)->file());
  }

  r_mint Env::method_line(rmethod cm) {
    return i(cm)->start_line(private_->state());
  }

  r_mint Env::method_id(rmethod meth) {
    CompiledMethod* cm = i(meth);

    if(VMMethod* vmm = cm->backend_method()) {
      return (vmm->method_id() << 1) | 1;
    }

    return 0;
  }

  rtable Env::table_new() {
    return o(LookupTable::create(private_->state()));
  }

  robject Env::table_fetch(rtable tbl, robject key, bool* fetched) {
    return s(i(tbl)->fetch(private_->state(), (Object*)key, fetched));
  }

  void Env::table_store(rtable tbl, robject key, robject val) {
    i(tbl)->store(private_->state(), (Object*)key, (Object*)val);
  }

  rarray Env::array_new(int size) {
    return o(Array::create(private_->state(), size));
  }

  void Env::array_set(rarray ary, int idx, robject obj) {
    i(ary)->set(private_->state(), idx, (Object*)obj);
  }

  robject Env::array_get(rarray ary, int idx) {
    return s(i(ary)->get(private_->state(), idx));
  }

  uint64_t Env::time_current_ns() {
    return get_current_time();
  }

  rsymbol Env::module_name(rmodule mod) {
    Module* module = i(mod);

    if(IncludedModule* im = try_as<IncludedModule>(module)) {
      return o(im->module()->name());
    } else {
      return o(module->name());
    }
  }

  bool Env::module_is_metaclass(rmodule mod) {
    if(try_as<SingletonClass>(i(mod))) {
      return true;
    }

    return false;
  }

  robject Env::metaclass_attached_instance(rmodule mod) {
    if(SingletonClass* mc = try_as<SingletonClass>(i(mod))) {
      return s(mc->attached_instance());
    }

    return NULL;
  }

  void Env::set_tool_enter_method(enter_method func) {
    private_->global()->set_tool_enter_method(func);
  }

  void Env::set_tool_leave_method(leave_func func) {
    private_->global()->set_tool_leave_method(func);
  }

  void Env::set_tool_enter_block(enter_block func) {
    private_->global()->set_tool_enter_block(func);
  }

  void Env::set_tool_leave_block(leave_func func) {
    private_->global()->set_tool_leave_block(func);
  }

  void Env::set_tool_enter_gc(enter_gc func) {
    private_->global()->set_tool_enter_gc(func);
  }

  void Env::set_tool_leave_gc(leave_func func) {
    private_->global()->set_tool_leave_gc(func);
  }

  void Env::set_tool_enter_script(enter_script func) {
    private_->global()->set_tool_enter_script(func);
  }

  void Env::set_tool_leave_script(leave_func func) {
    private_->global()->set_tool_leave_script(func);
  }

  void Env::set_tool_results(results_func func) {
    private_->global()->set_tool_results(func);
  }

  void Env::set_tool_enable(enable_func func) {
    private_->global()->set_tool_enable(func);
  }

  void Env::set_tool_shutdown(shutdown_func func) {
    private_->global()->set_tool_shutdown(func);
  }

  void Env::set_tool_thread_start(thread_start_func func) {
    private_->global()->set_tool_thread_start(func);
  }

  void Env::set_tool_thread_stop(thread_stop_func func) {
    private_->global()->set_tool_thread_stop(func);
  }

  Env* create_env(STATE) {
    Env* env = new Env;
    env->private_ = new EnvPrivate(state);

    return env;
  }

  void destroy_env(Env* env) {
    delete env->private_;
    delete env;
  }
}
