#ifndef RBX_COMPILED_FILE_HPP
#define RBX_COMPILED_FILE_HPP

#include "prelude.hpp"

#include <string>
#include <istream>

namespace rubinius {

  class Object;
  class VM;

  class CompiledFile {
  public:
    std::string magic;
    uint64_t signature;
    int version;

  private:
    std::istream* stream;

  public:
    CompiledFile(std::string magic, uint64_t signature, int version,
                 std::istream* stream)
      : magic(magic)
      , signature(signature)
      , version(version)
      , stream(stream)
    {}

    static CompiledFile* load(std::istream& stream);
    Object* body(STATE);
    bool execute(STATE);
  };

}

#endif
