#include "builtin/class.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/variable_scope.hpp"
#include "call_frame.hpp"
#include "object_utils.hpp"

#include <iostream>

namespace rubinius {
  Object* CallFrame::last_match(STATE) {
    CallFrame* use = this->top_ruby_frame();;

    while(use && use->is_inline_block()) {
      CallFrame* yielder = use->previous;
      if(!yielder) return cNil;
      // This works because the creator is always one above
      // the yielder with inline blocks.
      use = yielder->previous;
    }

    if(!use) return cNil;
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

  void CallFrame::print_backtrace(STATE, int total, bool filter) {
    print_backtrace(state, std::cout, total, filter);
  }

  void CallFrame::print_backtrace(STATE, std::ostream& stream, int total, bool filter) {
    CallFrame* cf = this;

    int i = -1;

    while(cf) {
      i++;

      if(total > 0 && i == total) return;

      if(NativeMethodFrame* nmf = cf->native_method_frame()) {
        stream << static_cast<void*>(cf) << ": ";
        NativeMethod* nm = try_as<NativeMethod>(nmf->get_object(nmf->method()));
        if(nm && nm->name()->symbol_p()) {
          stream << "capi:" << nm->name()->debug_str(state) << " at ";
          stream << nm->file()->c_str(state);
        } else {
          stream << "unknown capi";
        }

        stream << std::endl;
        cf = cf->previous;
        continue;
      }

      if(!cf->compiled_code) {
        cf = cf->previous;
        continue;
      }

      if(filter && cf->compiled_code->kernel_method(state)) {
        cf = cf->previous;
        continue;
      }

      stream << static_cast<void*>(cf) << ": ";

      if(cf->is_block_p(state)) {
        stream << "__block__";
      } else {
        if(SingletonClass* sc = try_as<SingletonClass>(cf->module())) {
          Object* obj = sc->attached_instance();

          if(Module* mod = try_as<Module>(obj)) {
            stream << mod->debug_str(state) << ".";
          } else {
            if(obj == G(main)) {
              stream << "MAIN.";
            } else {
              stream << "#<" << obj->class_object(state)->debug_str(state) <<
                        ":" << (void*)obj->id(state)->to_native() << ">.";
            }
          }
        } else if(IncludedModule* im = try_as<IncludedModule>(cf->module())) {
          stream <<  im->module()->debug_str(state) << "#";
        } else {
          Symbol* name;
          std::string mod_name;

          if(cf->module()->nil_p()) {
            mod_name = cf->constant_scope()->module()->debug_str(state);
          } else {
            if((name = try_as<Symbol>(cf->module()->module_name()))) {
              mod_name = name->debug_str(state);
            } else if((name = try_as<Symbol>(
                      cf->constant_scope()->module()->module_name()))) {
              mod_name = name->debug_str(state);
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
          stream << cf->compiled_code->name()->debug_str(state);
        }
      }

      stream << " in ";
      if(Symbol* file_sym = try_as<Symbol>(cf->compiled_code->file())) {
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
      cf = cf->previous;
    }

  }

  Symbol* CallFrame::file(STATE) {
    if(compiled_code) {
      return compiled_code->file();
    } else {
      return nil<Symbol>();
    }
  }

  int CallFrame::line(STATE) {
    if(!compiled_code) return -2;        // trampoline context
    return compiled_code->line(state, ip());
  }

  // Walks the CallFrame list to see if +scope+ is still running
  bool CallFrame::scope_still_valid(VariableScope* scope) {
    CallFrame* cur = this;
    while(cur) {
      if(cur->scope && cur->scope->on_heap() == scope) return true;
      cur = cur->previous;
    }

    return false;
  }

  void CallFrame::jit_fixup(STATE, CallFrame* creator) {
    VariableScope* parent = creator->promote_scope(state);
    scope->set_parent(parent);
    scope->set_block(creator->scope->block());
  }

  void CallFrame::dump() {
    VM* vm = VM::current();
    State state_obj(vm), *state = &state_obj;

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
      std::cout << "name=" << compiled_code->name()->debug_str(state) << " ";
    }

    std::cout << "ip=" << ip_ << " ";

    std::cout << "line=" << line(state);

    std::cout << ">\n";
  }

  Object* CallFrame::find_breakpoint(STATE) {
    if(!compiled_code) return 0;

    LookupTable* tbl = compiled_code->breakpoints();
    if(tbl->nil_p()) return 0;

    bool found = false;

    Object* obj = tbl->fetch(state, Fixnum::from(ip()), &found);
    if(found) return obj;

    return 0;
  }

  /* For debugging. */
  extern "C" {
    void __printbt__(CallFrame* call_frame) {
      State state(VM::current());
      call_frame->print_backtrace(&state);
    }
  }
}
