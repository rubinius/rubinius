/* A CompiledFile represents a .rbc. This class understands the layout
 * of a .rbc file. It can validate and load the body into a CompiledMethod
 * object. 
 *
 * CompiledFile::execute is a root stack frame in Rubinius. It's where
 * primary execution begins, when the VM loads the loader.rbc and executes
 * it. */

#include "compiled_file.hpp"
#include "marshal.hpp"
#include "objectmemory.hpp"
#include "builtin/task.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/class.hpp"

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

  OBJECT CompiledFile::body(STATE) {
    UnMarshaller mar(state, *stream);
    return mar.unmarshal();
  }

  bool CompiledFile::execute(STATE) {
    GO(current_task).set(state->new_task());
    TypedRoot<CompiledMethod*> cm(state, as<CompiledMethod>(body(state)));

    Message msg(state);
    msg.total_args = 0;
    msg.recv = G(main);
    msg.name = cm->name;
    msg.module = G(object);
    msg.name = cm->name;

    SET(cm.get(), scope, StaticScope::create(state));
    SET(cm.get()->scope, module, G(object));

    cm->execute(state, G(current_task), msg);
    try {
      G(current_task)->execute();
    } catch(Task::Halt &e) {
      return true;
    }

    // Task::execute contains the safe point, thus the above Task
    // and CompiledMethod pointers have likely been moved. DO NOT
    // USE THEM.

    return false;
  }
}
