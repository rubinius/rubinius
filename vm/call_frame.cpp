#include <iostream>

#include "call_frame.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/tuple.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/lookuptable.hpp"

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
    print_backtrace(state, std::cout);
  }

  void CallFrame::print_backtrace(STATE, std::ostream& stream) {
    CallFrame* cf = this;

    while(cf) {
      if(!cf->cm) {
        cf = static_cast<CallFrame*>(cf->previous);
        continue;
      }

      stream << static_cast<void*>(cf) << ": ";

      if(cf->is_block_p(state)) {
        stream << "__block__";
      } else {
        if(MetaClass* meta = try_as<MetaClass>(cf->module())) {
          if(Module* mod = try_as<Module>(meta->attached_instance())) {
            stream << mod->name()->c_str(state) << ".";
          } else {
            if(meta->attached_instance() == G(main)) {
              stream << "MAIN.";
            } else {
              stream << "#<" <<
                meta->attached_instance()->class_object(state)->name()->c_str(state) <<
                ":" << (void*)meta->attached_instance()->id(state)->to_native() << ">.";
            }
          }
        } else if(IncludedModule* im = try_as<IncludedModule>(cf->module())) {
          if(im->module()->name()->nil_p()) {
            stream << "<anonymous module>#";
          } else {
            stream << im->module()->name()->c_str(state) << "#";
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
          stream << mod_name << "#";
        }

        Symbol* name = try_as<Symbol>(cf->name());
        if(name) {
          stream << name->c_str(state);
        } else {
          stream << cf->cm->name()->c_str(state);
        }
      }

      stream << " in ";
      if(Symbol* file_sym = try_as<Symbol>(cf->cm->file())) {
        stream << file_sym->c_str(state) << ":" << cf->line(state);
      } else {
        stream << "<unknown>";
      }

      stream << " (+" << cf->ip();
      if(cf->is_inline_frame()) {
        stream << " inline";
      }
      stream << ")";

      stream << std::endl;
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

  Object* CallFrame::find_breakpoint(STATE) {
    LookupTable* tbl = cm->breakpoints();
    if(tbl->nil_p()) return 0;

    bool found = false;

    Object* obj = tbl->fetch(state, Fixnum::from(ip()), &found);
    if(found) return obj;

    return 0;
  }

  /* For debugging. */
  extern "C" {
    void __printbt__(CallFrame* call_frame) {
      call_frame->print_backtrace(VM::current_state());
    }
  }
}
