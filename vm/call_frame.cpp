#include <iostream>

#include "call_frame.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/tuple.hpp"
#include "builtin/staticscope.hpp"

#include "object_utils.hpp"

namespace rubinius {
  VariableScope* CallFrame::promote_scope_full(STATE) {
    return scope->create_heap_alias(state, this, !has_closed_scope_p());
  }

  VariableScope* CallFrame::method_scope(STATE) {
    VariableScope* current = promote_scope(state);
    if(!multiple_scopes_p()) return current;

    for(;;) {
      if(current->block_as_method_p()) return current;
      VariableScope* parent = current->parent();
      if(!parent->nil_p()) {
        current = parent;
      } else {
        return current;
      }
    }

    // Shouldn't ever get here.
    return 0;
  }

  void CallFrame::print_backtrace(STATE) {
    CallFrame* cf = this;

    while(cf) {
      if(!cf->cm) {
        cf = static_cast<CallFrame*>(cf->previous);
        continue;
      }

      std::cout << static_cast<void*>(cf) << ": ";

      if(cf->is_block_p(state)) {
        std::cout << "__block__";
      } else {
        if(MetaClass* meta = try_as<MetaClass>(cf->module())) {
          if(Module* mod = try_as<Module>(meta->attached_instance())) {
            std::cout << mod->name()->c_str(state) << ".";
          } else {
            std::cout << "#<" <<
              meta->attached_instance()->class_object(state)->name()->c_str(state) <<
              ":" << (void*)meta->attached_instance() << ">.";
          }
        } else {
          const char* mod_name;
          if(cf->module()->nil_p()) {
            mod_name = cf->cm->scope()->module()->name()->c_str(state);
          } else if(cf->module()->name()->nil_p()) {
            mod_name = cf->cm->scope()->module()->name()->c_str(state);
          } else {
            mod_name = cf->module()->name()->c_str(state);
          }
          std::cout << mod_name << "#";
        }

        Symbol* name = try_as<Symbol>(cf->name());
        if(name) {
          std::cout << name->c_str(state);
        } else {
          std::cout << cf->cm->name()->c_str(state);
        }
      }

      std::cout << " in ";
      if(Symbol* file_sym = try_as<Symbol>(cf->cm->file())) {
        std::cout << file_sym->c_str(state) << ":" << cf->line(state);
      } else {
        std::cout << "<unknown>";
      }

      std::cout << " (+" << cf->ip();
      if(cf->is_inline_frame()) {
        std::cout << " inline";
      }
      std::cout << ")";

      std::cout << std::endl;
      cf = static_cast<CallFrame*>(cf->previous);
    }

  }

  int CallFrame::line(STATE) {
    if(!cm) return -2;        // trampoline context
    return cm->line(state, ip());
  }

  // Walks the CallFrame list to see if +scope+ is still running
  bool CallFrame::scope_still_valid(VariableScope* scope) {
    CallFrame* cur = this;
    while(cur) {
      if(cur->scope->on_heap() == scope) return true;
      cur = static_cast<CallFrame*>(cur->previous);
    }

    return false;
  }

  void CallFrame::dump() {
    VM* state = VM::current_state();
    std::cout << "<CallFrame:" << (void*)this << " ";
    if(is_inline_frame()) {
      std::cout << "inline ";
    }

    if(is_block_p(state)) {
      std::cout << "block ";
    } else if(dispatch_data) {
      std::cout << "name=" << name()->c_str(state) << " ";
    } else {
      std::cout << "name=" << cm->name()->c_str(state) << " ";
    }

    std::cout << "ip=" << ip_ << " ";

    std::cout << "line=" << line(state);

    std::cout << ">\n";
  }

  /* For debugging. */
  extern "C" {
    void __printbt__(CallFrame* call_frame) {
      call_frame->print_backtrace(VM::current_state());
    }
  }
}
