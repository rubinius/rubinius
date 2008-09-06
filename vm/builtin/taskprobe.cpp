#include "builtin/class.hpp"
#include "builtin/contexts.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/iseq.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/taskprobe.hpp"

#include "objectmemory.hpp"
#include "vm.hpp"
#include "jit_state.h"

#include <iostream>
#include <iomanip>

namespace rubinius {

  void TaskProbe::init(STATE) {
    GO(taskprobe).set(state->new_class("TaskProbe", G(object),
                                       TaskProbe::fields, G(rubinius)));
    SET(G(taskprobe), name, state->symbol("Rubinius::TaskProbe"));
    G(taskprobe)->set_object_type(TaskProbe::type);
  }

  TaskProbe* TaskProbe::create(STATE) {
    TaskProbe* probe = (TaskProbe*)state->om->new_object(G(taskprobe),
                                                         TaskProbe::fields);
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
    }

    return 0;
  }

  OBJECT TaskProbe::enable(STATE, SYMBOL probe) {
    enable(std::string(probe->c_str(state)));
    return Qnil;
  }

  void TaskProbe::enable(std::string probe) {
    this->flags |= probe_flag(probe);
  }

  bool TaskProbe::enabled_p(size_t flag) {
    return (this->flags & flag) != 0;
  }

  OBJECT TaskProbe::disable(STATE, SYMBOL probe) {
    disable(std::string(probe->c_str(state)));
    return Qnil;
  }

  void TaskProbe::disable(std::string probe) {
    this->flags &= ~probe_flag(probe);
  }

  void TaskProbe::added_method(Task* task, Module* mod, SYMBOL name, CompiledMethod *meth) {
    if(enabled_p(PROBE_ADD_METHOD)) {
      std::cout << "[Added method '" <<
        name->to_str(task->state)->c_str() << "']" << std::endl;
    }
  }

  void TaskProbe::start_method(Task* task, Message& msg) {
    if(enabled_p(PROBE_START_METHOD)) {
      if(msg.send_site) {
        std::cout << "[Sending: '" <<
          msg.send_site->name->to_str(task->state)->c_str() <<
          "']" << std::endl;
      }
    }
  }

  void TaskProbe::lookup_failed(Task* task, Message& msg) {
    if(enabled_p(PROBE_LOOKUP_FAILED)) {
      std::cout << "[Unable to find: '" <<
        msg.send_site->name->to_str(task->state)->c_str() <<
        "']" << std::endl;
    }
  }

  void TaskProbe::execute_instruction(Task* task, MethodContext* ctx, opcode op) {
    if(enabled_p(PROBE_EXECUTE_INSTRUCTION)) {
      std::cout << std::left << std::setw(27) <<
        ctx->cm->name->to_str(task->state)->c_str() << "+" <<
        std::right << std::setw(4) << ctx->ip << ": " <<
        std::left << std::setw(30) <<
        InstructionSequence::get_instruction_name(op) << " " <<
        std::right << std::setw(4) << ctx->js.stack - ctx->stk << " " <<
        std::right << std::setw(10) << (void*) *ctx->js.stack << std::endl;
    }
  }

  void TaskProbe::Info::mark(OBJECT t, ObjectMark& mark) { }

}
