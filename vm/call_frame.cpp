#include <iostream>

#include "call_frame.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/tuple.hpp"
#include "object_utils.hpp"

namespace rubinius {
  void CallFrame::promote_scope(STATE) {
    if(scope->obj_type != InvalidType) return;
    VariableScope* new_scope = scope->promote(state);

    if(scope == top_scope) top_scope = new_scope;
    scope = new_scope;
  }

  MethodContext* CallFrame::create_context(STATE) {
    abort();
    return NULL;
  }

  void CallFrame::print_backtrace(STATE) {
    CallFrame* cf = this;

    while(cf) {
      if(!cf->cm) {
        cf = cf->previous;
        continue;
      }

      std::cout << static_cast<void*>(cf) << ": ";

      if(cf->is_block) {
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
          if(cf->module()->name()->nil_p()) {
            mod_name = "<unknown>";
          } else {
            mod_name = cf->module()->name()->c_str(state);
          }
          std::cout << mod_name << "#";
        }

        Symbol* name = try_as<Symbol>(cf->name);
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

      std::cout << std::endl;
      cf = cf->previous;
    }

  }

  int CallFrame::line(STATE) {
    if(!cm) return -2;        // trampoline context
    if(cm->lines()->nil_p()) return -3;

    for(size_t i = 0; i < cm->lines()->num_fields(); i++) {
      Tuple* entry = as<Tuple>(cm->lines()->at(state, i));

      Fixnum* start_ip = as<Fixnum>(entry->at(state, 0));
      Fixnum* end_ip   = as<Fixnum>(entry->at(state, 1));
      Fixnum* line     = as<Fixnum>(entry->at(state, 2));

      if(start_ip->to_native() <= ip && end_ip->to_native() >= ip)
        return line->to_native();
    }

    return -1;
  }
}
