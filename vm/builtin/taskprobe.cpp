#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/iseq.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/taskprobe.hpp"

#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "vm.hpp"
#include "jit_state.h"
#include "arguments.hpp"
#include "dispatch.hpp"

#include <iostream>
#include <iomanip>

namespace rubinius {

  void TaskProbe::init(STATE) {
    GO(taskprobe).set(state->new_class("TaskProbe", G(object),
                                        G(rubinius)));
    G(taskprobe)->name(state, state->symbol("Rubinius::TaskProbe"));
    G(taskprobe)->set_object_type(state, TaskProbe::type);
  }

  TaskProbe* TaskProbe::create(STATE) {
    TaskProbe* probe = state->new_object<TaskProbe>(G(taskprobe));
    probe->flags = 0;
    return probe;
  }

  bool TaskProbe::parse_env(const char* env) {
    size_t back;
    size_t front = 0;

    if(!env) env = (const char*)getenv("PROBE");
    if(!env) return false;

    std::string probes = std::string(env);

    while((back = probes.find_first_of(",", front)) != std::string::npos) {
      enable(probes.substr(front, back - front));
      front = back + 1;
    }

    if(front != std::string::npos && back == std::string::npos) {
      enable(probes.substr(front));
    }

    return this->flags != 0;
  }

  size_t TaskProbe::probe_flag(std::string probe) {
    if(probe == PROBE_ALL_OP || probe == PROBE_ALL1_OP) {
      return PROBE_ALL;
    } else if(probe == PROBE_ADD_METHOD_OP) {
      return PROBE_ADD_METHOD;
    } else if(probe == PROBE_START_METHOD_OP) {
      return PROBE_START_METHOD;
    } else if(probe == PROBE_LOOKUP_FAILED_OP) {
      return PROBE_LOOKUP_FAILED;
    } else if(probe == PROBE_EXECUTE_INSTRUCTION_OP) {
      return PROBE_EXECUTE_INSTRUCTION;
    } else if(probe == PROBE_LOAD_RUNTIME_OP) {
      return PROBE_LOAD_RUNTIME;
    } else if(probe == PROBE_PRIMITIVES_OP) {
      return PROBE_PRIMITIVES;
    } else if(probe == PROBE_EXECUTE_METHOD_OP) {
      return PROBE_EXECUTE_METHOD;
    }

    return 0;
  }

  Object* TaskProbe::enable(STATE, Symbol* probe) {
    enable(std::string(probe->c_str(state)));
    return Qnil;
  }

  void TaskProbe::enable(std::string probe) {
    this->flags |= probe_flag(probe);
  }

  Object* TaskProbe::enabled_p(STATE, Symbol* probe) {
    if(enabled_p(probe_flag(std::string(probe->c_str(state))))) {
      return Qtrue;
    }

    return Qfalse;
  }

  bool TaskProbe::enabled_p(size_t flag) {
    return (this->flags & flag) != 0;
  }

  Object* TaskProbe::disable(STATE, Symbol* probe) {
    disable(std::string(probe->c_str(state)));
    return Qnil;
  }

  void TaskProbe::disable(std::string probe) {
    this->flags &= ~probe_flag(probe);
  }

  /** @todo Fix, Task is gone, CM will be gone. --rue */
  void TaskProbe::added_method(Task* task, Module* mod, Symbol* name, CompiledMethod *meth) {
//    if(enabled_p(PROBE_ADD_METHOD)) {
//      std::cout << "[Added method '" <<
//        name->c_str(task->state) << "']" << std::endl;
//    }
  }

  /** @todo Fix, Task is gone, CM will be gone. --rue */
  void TaskProbe::start_method(Task* task, Message& msg) {
//    if(enabled_p(PROBE_START_METHOD)) {
//      if(msg.send_site) {
//        std::cout << "[Sending: '" <<
//          msg.send_site->name()->c_str(task->state) <<
//          "']" << std::endl;
//      }
//    }
  }

  /** @todo Fix, Task is gone, CM will be gone. --rue */
  void TaskProbe::execute_method(STATE, Task* task, Message& msg) {
//    if(enabled_p(PROBE_EXECUTE_METHOD)) {
//      std::cout << "[Executing: '";
//
//      if(MetaClass* meta = try_as<MetaClass>(msg.module)) {
//        if(Module* mod = try_as<Module>(meta->attached_instance())) {
//          std::cout << mod->name()->c_str(state) << ".";
//        } else {
//          std::cout << "#<" <<
//            meta->attached_instance()->class_object(state)->name()->c_str(state) <<
//            ":" << (void*)meta->attached_instance() << ">.";
//        }
//      } else {
//        if(msg.module->name()->nil_p()) {
//          std::cout << "<anonymous>#";
//        } else {
//          std::cout << msg.module->name()->c_str(state) << "#";
//        }
//      }
//
//      std::cout << msg.name->c_str(state) << "']" << std::endl;
//    }
  }

  /** @todo Fix, Task is gone, CM will be gone. --rue */
  void TaskProbe::lookup_failed(Task* task, Message& msg) {
//    if(enabled_p(PROBE_LOOKUP_FAILED)) {
//      std::cout << "[Unable to find: '" <<
//        msg.send_site->name()->c_str(task->state) <<
//        "']" << std::endl;
//    }
  }

  /** @todo Fix, Task is gone, CM will be gone. --rue */
  void TaskProbe::execute_instruction(Task* task, MethodContext* ctx, opcode op) {
//    if(enabled_p(PROBE_EXECUTE_INSTRUCTION)) {
//      const char* name;
//      if(ctx->cm()->name()->nil_p()) {
//        name = "<unknown>";
//      } else {
//        name = ctx->cm()->name()->c_str(task->state);
//      }
//
//      std::cout << std::left << std::setw(27) <<
//        name << "+" <<
//        std::right << std::setw(4) << ctx->ip << ": " <<
//        std::left << std::setw(30) <<
//        InstructionSequence::get_instruction_name(op) << " ";
//
//      int stack_pos = ctx->js.stack - ctx->stk;
//      std::cout << std::right << std::setw(4) << stack_pos;
//      if(stack_pos >= 0) {
//        std::cout << " " << std::right << std::setw(10) <<
//          (void*) *ctx->js.stack;
//      }
//
//      std::cout << std::endl;
//
//      if(stack_pos >= 0) {
//        Object* top = *ctx->js.stack;
//        if(top->reference_p()) {
//          ObjectPosition pos = task->state->om->validate_object(top);
//          assert(pos != cInWrongYoungHalf);
//          assert(pos != cUnknown);
//        }
//      }
//    }
  }

  void TaskProbe::load_runtime(STATE, std::string file) {
    if(enabled_p(PROBE_LOAD_RUNTIME)) {
      std::cout << "[Loading: " << file << "]" << std::endl;
    }
  }

  void TaskProbe::missing_primitive(STATE, std::string prim) {
    if(enabled_p(PROBE_PRIMITIVES)) {
      std::cout << "[Unable to resolve primitive: " << prim << "]" << std::endl;
    }
  }

  void TaskProbe::Info::mark(Object* t, ObjectMark& mark) { }

}
