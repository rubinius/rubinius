/* A CompiledFile represents a .rbc. This class understands the layout
 * of a .rbc file. It can validate and load the body into a CompiledMethod
 * object.
 *
 * CompiledFile::execute is a root stack frame in Rubinius. It's where
 * primary execution begins, when the VM loads the loader.rbc and executes
 * it. */

#include "compiled_file.hpp"
#include "marshal.hpp"
#include "message.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"

#include "builtin/task.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/class.hpp"
#include "builtin/thread.hpp"

#include "exception_point.hpp"

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
    TypedRoot<CompiledMethod*> cm(state, as<CompiledMethod>(body(state)));

    CallFrame cf;
    cf.previous = NULL;
    cf.name = NULL;
    cf.cm = NULL;
    cf.top_scope = NULL;
    cf.scope = NULL;
    cf.stack_size = 0;
    cf.current_unwind = 0;
    cf.ip = 0;

    GlobalLock::LockGuard lock(state->global_lock());

    Message msg(state);
    msg.setup(NULL, G(main), &cf, 0);
    msg.name = cm->name();
    msg.module = G(object);
    msg.method = cm.get();

    cm.get()->scope(state, StaticScope::create(state));
    cm.get()->scope()->module(state, G(object));

    cm->execute(state, &cf, msg);

    return true;
  }
}
