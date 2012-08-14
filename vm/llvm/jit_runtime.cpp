#ifdef ENABLE_LLVM


#include "llvm/jit_runtime.hpp"
#include "llvm/state.hpp"

#include "gc/code_manager.hpp"
#include "gc/gc.hpp"

#include "builtin/symbol.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/module.hpp"

namespace rubinius {
namespace jit {

  RuntimeDataHolder::~RuntimeDataHolder() {
    runtime_data_.clear();
  }

  void RuntimeDataHolder::cleanup(State* state, CodeManager* cm) {
    LLVMState* ls = cm->shared()->llvm_state;
    assert(ls);

    if(ls->config().jit_removal_print) {
      void* fin = (void*)((intptr_t)native_func_ + native_size_);

      std::cout << "Remove function: " << function_ << " / "
                << native_func_ << "-" << fin
                << "\n";
    }

    ls->remove(function_);
  }

  void RuntimeDataHolder::mark_all(Object* obj, ObjectMark& mark) {
    Object* tmp;

    for(std::list<jit::RuntimeData*>::iterator i = runtime_data_.begin();
        i != runtime_data_.end();
        ++i) {
      jit::RuntimeData* rd = *i;

      if(rd->method()) {
        tmp = mark.call(rd->method());
        if(tmp) {
          rd->method_ = (CompiledCode*)tmp;
          if(obj) mark.just_set(obj, tmp);
        }
      }

      if(rd->name()) {
        tmp = mark.call(rd->name());
        if(tmp) {
          rd->name_ = (Symbol*)tmp;
          if(obj) mark.just_set(obj, tmp);
        }
      }

      if(rd->module()) {
        tmp = mark.call(rd->module());
        if(tmp) {
          rd->module_ = (Module*)tmp;
          if(obj) mark.just_set(obj, tmp);
        }
      }
    }
  }

  void RuntimeDataHolder::run_write_barrier(gc::WriteBarrier* wb, Object* obj) {
    for(std::list<jit::RuntimeData*>::iterator i = runtime_data_.begin();
        i != runtime_data_.end();
        ++i) {
      jit::RuntimeData* rd = *i;

      if(rd->method()) {
        obj->write_barrier(wb, rd->method());
      }

      if(rd->name()) {
        obj->write_barrier(wb, rd->name());
      }

      if(rd->module()) {
        obj->write_barrier(wb, rd->module());
      }
    }

  }
}}
#endif
