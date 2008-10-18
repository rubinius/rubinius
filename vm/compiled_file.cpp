/* A CompiledFile represents a .rbc. This class understands the layout
 * of a .rbc file. It can validate and load the body into a CompiledMethod
 * object.
 *
 * CompiledFile::execute is a root stack frame in Rubinius. It's where
 * primary execution begins, when the VM loads the loader.rbc and executes
 * it. */

#include "compiled_file.hpp"
#include "context_cache.hpp"
#include "marshal.hpp"
#include "message.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"

#include "builtin/task.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/class.hpp"
#include "builtin/thread.hpp"


namespace rubinius {
  CompiledFile* CompiledFile::load(std::istream& stream) {
    std::string magic, sum;
    long ver;

    stream >> magic;
    stream >> ver;
    stream >> sum;
    stream.get(); // eat the \n

    return new CompiledFile(magic, ver, sum, &stream);
  }

  Object* CompiledFile::body(STATE) {
    UnMarshaller mar(state, *stream);
    return mar.unmarshal();
  }

  bool CompiledFile::execute(STATE) {
    Task* task = state->new_task();
    TypedRoot<CompiledMethod*> cm(state, as<CompiledMethod>(body(state)));

    Message msg(state);
    msg.setup(NULL, G(main), task->active(), 0, 0);
    msg.name = cm->name();
    msg.module = G(object);
    msg.method = cm.get();

    G(current_thread)->task(state, task);
    state->activate_task(task);

    cm.get()->scope(state, StaticScope::create(state));
    cm.get()->scope()->module(state, G(object));

    cm->execute(state, G(current_task), msg);
    try {
      state->run_and_monitor();
    } catch(Task::Halt &e) {
      return true;
    }

    // Task::execute contains the safe point, thus the above Task
    // and CompiledMethod pointers have likely been moved. DO NOT
    // USE THEM.

    return false;
  }
}
