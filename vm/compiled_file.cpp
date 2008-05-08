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
}
