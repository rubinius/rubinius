/* A CompiledFile represents a .rbc. This class understands the layout
 * of a .rbc file. It can validate and load the body into a CompiledMethod
 * object.
 *
 * CompiledFile::execute is a root stack frame in Rubinius. It's where
 * primary execution begins, when the VM loads the loader.rbc and executes
 * it. */

#include "compiled_file.hpp"
#include "marshal.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/class.hpp"
#include "builtin/thread.hpp"

namespace rubinius {
  CompiledFile* CompiledFile::load(std::istream& stream) {
    std::string magic;
    uint64_t signature;
    int version;

    stream >> magic;
    stream >> signature;
    stream >> version;
    stream.get(); // eat the \n

    return new CompiledFile(magic, signature, version, &stream);
  }

  Object* CompiledFile::body(STATE) {
    UnMarshaller mar(state, *stream);
    return mar.unmarshal();
  }

  bool CompiledFile::execute(STATE) {
    TypedRoot<CompiledMethod*> cm(state, as<CompiledMethod>(body(state)));

    state->thread_state()->clear();

    Arguments args(state->symbol("script"), G(main), 0, 0);

    cm.get()->scope(state, StaticScope::create(state));
    cm.get()->scope()->module(state, G(object));

    cm->execute(state, NULL, cm.get(), G(object), args);

    return true;
  }
}
