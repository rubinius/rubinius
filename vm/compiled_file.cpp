/* A CompiledFile represents a .rbc. This class understands the layout
 * of a .rbc file. It can validate and load the body into a CompiledCode
 * object.
 *
 * CompiledFile::execute is a root stack frame in Rubinius. It's where
 * primary execution begins, when the VM loads the loader.rbc and executes
 * it. */

#include "arguments.hpp"
#include "builtin/constant_scope.hpp"
#include "builtin/compiled_code.hpp"
#include "builtin/class.hpp"
#include "builtin/thread.hpp"
#include "call_frame.hpp"
#include "compiled_file.hpp"
#include "marshal.hpp"
#include "object_memory.hpp"
#include "object_utils.hpp"

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
    TypedRoot<CompiledCode*> code(state, as<CompiledCode>(body(state)));

    state->thread_state()->clear();

    Arguments args(state->symbol("script"), G(main), 0, 0);

    code.get()->scope(state, ConstantScope::create(state));
    code.get()->scope()->module(state, G(object));

    code->execute(state, NULL, code.get(), G(object), args);

    return true;
  }
}
