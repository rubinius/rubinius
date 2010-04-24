#ifdef ENABLE_LLVM
#include "llvm/jit_runtime.hpp"
#include "gc/code_manager.hpp"
#include "llvm/jit.hpp"

#include "gc/gc.hpp"

#include "builtin/symbol.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/module.hpp"

namespace rubinius {
namespace jit {

  void RuntimeDataHolder::cleanup(CodeManager* cm) {
    LLVMState* ls = cm->shared()->llvm_state;
    assert(ls);

    ls->remove(function_);
  }

  void RuntimeDataHolder::mark_all(Object* obj, ObjectMark& mark) {
    Object* tmp;

    for(std::list<jit::RuntimeData*>::iterator i = runtime_data_.begin();
        i != runtime_data_.end();
        i++) {
      jit::RuntimeData* rd = *i;

      tmp = mark.call(rd->method());
      if(tmp) {
        rd->method_ = (CompiledMethod*)tmp;
        if(obj) mark.just_set(obj, tmp);
      }

      tmp = mark.call(rd->name());
      if(tmp) {
        rd->name_ = (Symbol*)tmp;
        if(obj) mark.just_set(obj, tmp);
      }

      tmp = mark.call(rd->module());
      if(tmp) {
        rd->module_ = (Module*)tmp;
        if(obj) mark.just_set(obj, tmp);
      }
    }
  }

  void RuntimeDataHolder::visit_all(ObjectVisitor& visit) {
    for(std::list<jit::RuntimeData*>::iterator i = runtime_data_.begin();
        i != runtime_data_.end();
        i++) {
      jit::RuntimeData* rd = *i;

      visit.call(rd->method());
      visit.call(rd->name());
      visit.call(rd->module());
    }

  }

  void RuntimeDataHolder::run_write_barrier(gc::WriteBarrier* wb, Object* obj) {
    for(std::list<jit::RuntimeData*>::iterator i = runtime_data_.begin();
        i != runtime_data_.end();
        i++) {
      jit::RuntimeData* rd = *i;

      obj->write_barrier(wb, rd->method());
      obj->write_barrier(wb, rd->name());
      obj->write_barrier(wb, rd->module());
    }

  }
}}
#endif
