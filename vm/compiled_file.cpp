#include "compiled_file.hpp"
#include "marshal.hpp"

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
    TypedRoot<Task*> task(state, Task::create(state));
    TypedRoot<CompiledMethod*> cm(state, as<CompiledMethod>(body(state)));

    Message msg(state);
    msg.args = 0;
    msg.recv = G(main);

    SET(cm.get(), scope, StaticScope::create(state));
    SET(cm.get()->scope, module, G(object));

    cm->execute(state, task.get(), msg);
    try {
      task->execute();
    } catch(Task::Halt &e) {
      return true;
    }

    // Task::execute contains the safe point, thus the above Task
    // and CompiledMethod pointers have likely been moved. DO NOT
    // USE THEM.

    return false;
  }
}
