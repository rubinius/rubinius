#include "memory.hpp"
#include "call_frame.hpp"
#include "object_utils.hpp"
#include "machine_code.hpp"

#include "class/class.hpp"
#include "class/compiled_code.hpp"
#include "class/lexical_scope.hpp"
#include "class/lookup_table.hpp"
#include "class/native_method.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"
#include "class/variable_scope.hpp"
#include "class/unwind_site.hpp"

#include "capi/capi.hpp"

#include "diagnostics/machine.hpp"

#include <iostream>

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

  void CallFrame::push_unwind(UnwindSite* unwind_site) {
    unwind_site->previous(unwind);
    unwind = unwind_site;
  }

  UnwindSite* CallFrame::pop_unwind() {
    if(UnwindSite* unwind_site = unwind) {
      unwind = unwind_site->previous();
      unwind_site->previous(nullptr);
      return unwind_site;
    } else {
      return nullptr;
    }
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
        NativeMethod* nm = MemoryHandle::try_as<NativeMethod>(nmf->method());
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

      if(filter && cf->compiled_code->core_method(state)) {
        cf = cf->previous;
        continue;
      }

      stream << static_cast<void*>(cf) << ": ";

      if(cf->is_block_p(state)) {
        stream << "__block__";
      } else {
        if(SingletonClass* sc = try_as<SingletonClass>(cf->module())) {
          Object* obj = sc->singleton();

          if(Module* mod = try_as<Module>(obj)) {
            stream << mod->debug_str(state) << ".";
          } else {
            if(obj == G(main)) {
              stream << "MAIN.";
            } else {
              stream << "#<" << obj->class_object(state)->debug_str(state) <<
                        ":" << (void*)obj->object_id(state)->to_native() << ">.";
            }
          }
        } else if(IncludedModule* im = try_as<IncludedModule>(cf->module())) {
          stream <<  im->module()->debug_str(state) << "#";
        } else {
          Symbol* name;
          std::string mod_name;

          if(cf->module()->nil_p()) {
            mod_name = cf->lexical_scope()->module()->debug_str(state);
          } else {
            if((name = try_as<Symbol>(cf->module()->module_name()))) {
              mod_name = name->debug_str(state);
            } else if((name = try_as<Symbol>(
                      cf->lexical_scope()->module()->module_name()))) {
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

  void CallFrame::jit_fixup(STATE, CallFrame* creator) {
    VariableScope* parent = creator->promote_scope(state);
    scope->set_parent(parent);
    scope->set_block(creator->scope->block());
  }

  void CallFrame::dump() {
    ThreadState* state = ThreadState::current();

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
}
