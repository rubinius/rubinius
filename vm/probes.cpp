
#include <iostream>
#include <iomanip>

#include "probes.hpp"
#include "builtin/contexts.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/iseq.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "message.hpp"
#include "jit_state.h"

namespace rubinius {
  void TaskProbeFlags::set_all_flags() {
    this->all_flags = 0xffffffff;
  }

  bool TaskProbeFlags::exists(std::string options, const char* option) {
    return options.find(option) != std::string::npos;
  }

  void TaskProbeFlags::parse(std::string options) {
    if(options == "1") {
      set_all_flags();
      return;
    }

    if(exists(options, "add_method"))          this->AddMethod = true;
    if(exists(options, "start_method"))        this->StartMethod = true;
    if(exists(options, "lookup_failed"))       this->LookupFailed = true;
    if(exists(options, "execute_instruction")) this->ExecuteInstruction = true;
  }

  TaskProbe::TaskProbe() {
    flags.all_flags = 0;
    char* options = getenv("PROBE");

    if(!options) return;

    flags.parse(std::string(options));
  }

  void TaskProbe::added_method(Task* task, Module* mod, SYMBOL name, CompiledMethod *meth) {
    if(flags.AddMethod) {
      std::cout << "[Added method '" <<
        name->to_str(task->state)->c_str() << "']" << std::endl;
    }
  }

  void TaskProbe::start_method(Task* task, Message& msg) {
    if(flags.StartMethod) {
      if(msg.send_site) {
        std::cout << "[Sending: '" <<
          msg.send_site->name->to_str(task->state)->c_str() <<
          "']" << std::endl;
      }
    }
  }

  void TaskProbe::lookup_failed(Task* task, Message& msg) {
    if(flags.LookupFailed) {
      std::cout << "[Unable to find: '" <<
        msg.send_site->name->to_str(task->state)->c_str() <<
        "']" << std::endl;
    }
  }

  void TaskProbe::execute_instruction(Task* task, MethodContext* ctx, opcode op) {
    if(flags.ExecuteInstruction) {
      std::cout << std::left << std::setw(27) <<
        ctx->cm->name->to_str(task->state)->c_str() << "+" <<
        std::right << std::setw(4) << ctx->ip << ": " <<
        std::left << std::setw(30) <<
        InstructionSequence::get_instruction_name(op) << " " <<
        std::right << std::setw(4) << ctx->js.stack - ctx->stk << " " <<
        std::right << std::setw(10) << (void*) *ctx->js.stack << std::endl;
    }
  }
}
