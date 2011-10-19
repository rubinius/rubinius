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
#include "builtin/nativemethod.hpp"

#include "object_utils.hpp"

namespace rubinius {
  Object* CallFrame::last_match(STATE) {
    CallFrame* use = this->top_ruby_frame();;

    while(use && use->is_inline_block()) {
      CallFrame* yielder = use->previous;
      if(!yielder) return Qnil;
      // This works because the creator is always one above
      // the yielder with inline blocks.
      use = yielder->previous;
    }

    if(!use) return Qnil;
    return use->scope->last_match(state);
  }

  void CallFrame::set_last_match(STATE, Object* obj) {
    CallFrame* use = this->top_ruby_frame();

    while(use && use->is_inline_block()) {
      CallFrame* yielder = use->previous;
      if(!yielder) return;
      // This works because the creator is always one above
      // the yielder with inline blocks.
      use = yielder->previous;
    }

    if(!use) return;
    use->scope->set_last_match(state, obj);
  }

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

  void CallFrame::print_backtrace(STATE, int total) {
    print_backtrace(state, std::cout, total);
  }

  void CallFrame::print_backtrace(STATE, std::ostream& stream, int total) {
    CallFrame* cf = this;

    int i = -1;

    while(cf) {
      i++;

      if(total > 0 && i == total) return;
      stream << static_cast<void*>(cf) << ": ";

      if(NativeMethodFrame* nmf = cf->native_method_frame()) {
        NativeMethod* nm = try_as<NativeMethod>(nmf->get_object(nmf->method()));
        if(nm || !nm->name()->symbol_p()) {
          stream << "capi:" << nm->name()->debug_str(state) << " at ";
          stream << nm->file()->c_str(state);
        } else {
          stream << "unknown capi";
        }

        stream << std::endl;
        cf = static_cast<CallFrame*>(cf->previous);
        continue;
      }

      if(!cf->cm) {
        cf = static_cast<CallFrame*>(cf->previous);
        continue;
      }

      if(cf->is_block_p(state)) {
        stream << "__block__";
      } else {
        if(SingletonClass* sc = try_as<SingletonClass>(cf->module())) {
          if(Module* mod = try_as<Module>(sc->attached_instance())) {
            stream << mod->name()->debug_str(state) << ".";
          } else {
            if(sc->attached_instance() == G(main)) {
              stream << "MAIN.";
            } else {
              stream << "#<" <<
                sc->attached_instance()->class_object(state)->name()->debug_str(state) <<
                ":" << (void*)sc->attached_instance()->id(state)->to_native() << ">.";
            }
          }
        } else if(IncludedModule* im = try_as<IncludedModule>(cf->module())) {
          if(im->module()->name()->nil_p()) {
            stream << "<anonymous module>#";
          } else {
            stream << im->module()->name()->debug_str(state) << "#";
          }
        } else {
          std::string mod_name;
          if(cf->module()->nil_p()) {
            mod_name = cf->cm->scope()->module()->name()->debug_str(state);
          } else {
            if(Symbol* s = try_as<Symbol>(cf->module()->name())) {
              mod_name = s->debug_str(state);
            } else if(Symbol* s = try_as<Symbol>(cf->cm->scope()->module()->name())) {
              mod_name = s->debug_str(state);
            } else {
              mod_name = "<anonymous module>";
            }
          }
          stream << mod_name << "#";
        }

        Symbol* name = try_as<Symbol>(cf->name());
        if(name) {
          stream << name->debug_str(state);
        } else {
          stream << cf->cm->name()->debug_str(state);
        }
      }

      stream << " in ";
      if(Symbol* file_sym = try_as<Symbol>(cf->cm->file())) {
        stream << file_sym->debug_str(state) << ":" << cf->line(state);
      } else {
        stream << "<unknown>";
      }

      stream << " (+" << cf->ip();
      if(cf->is_inline_frame()) {
        stream << " inline";
      } else if(cf->jitted_p()) {
        stream << " jit";
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
      if(cur->scope && cur->scope->on_heap() == scope) return true;
      cur = static_cast<CallFrame*>(cur->previous);
    }

    return false;
  }

  void CallFrame::dump() {
    VM* state = VM::current();
    std::cout << "<CallFrame:" << (void*)this << " ";

    if(native_method_p()) {
      std::cout << "capi>\n";
      return;
    }

    if(is_inline_frame()) {
      std::cout << "inline ";
    }

    if(is_block_p(state)) {
      std::cout << "block ";
    } else if(dispatch_data) {
      std::cout << "name=" << name()->debug_str(state) << " ";
    } else {
      std::cout << "name=" << cm->name()->debug_str(state) << " ";
    }

    std::cout << "ip=" << ip_ << " ";

    std::cout << "line=" << line(state);

    std::cout << ">\n";
  }

  Object* CallFrame::find_breakpoint(STATE) {
    if(!cm) return 0;

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
      call_frame->print_backtrace(VM::current());
    }
  }
}
