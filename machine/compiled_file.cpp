/* A CompiledFile represents a .rbc. This class understands the layout
 * of a .rbc file. It can validate and load the body into a CompiledCode
 * object.
 *
 * CompiledFile::execute is a root stack frame in Rubinius. It's where
 * primary execution begins, when the VM loads the loader.rbc and executes
 * it. */

#include "arguments.hpp"
#include "class/lexical_scope.hpp"
#include "class/compiled_code.hpp"
#include "class/class.hpp"
#include "class/thread.hpp"
#include "call_frame.hpp"
#include "compiled_file.hpp"
#include "marshal.hpp"
#include "memory.hpp"
#include "object_utils.hpp"

#include "instruments/timing.hpp"


namespace rubinius {
  CompiledFile* CompiledFile::load(STATE, std::istream& stream) {
    timer::StopWatch<timer::microseconds> timer(
        state->vm()->metrics().machine.bytecode_load_us);

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
    memory::TypedRoot<CompiledCode*> code(state, as<CompiledCode>(body(state)));

    state->thread_state()->clear();

    Arguments args(state->symbol("script"), G(main));

    code.get()->scope(state, LexicalScope::create(state));
    code.get()->scope()->module(state, G(object));

    code->execute(state, code.get(), G(object), args);

    return true;
  }
}
