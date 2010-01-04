#ifndef RBX_COMPILED_FILE_HPP
#define RBX_COMPILED_FILE_HPP

#include <string>

#include "prelude.hpp"

namespace rubinius {

  class Object;
  class VM;

  class CompiledFile {
  public:
    std::string magic;
    uint64_t version;
    std::string sum;

  private:
    std::istream* stream;

  public:
    CompiledFile(std::string magic, uint64_t version, std::string sum,
                 std::istream* stream)
      : magic(magic)
      , version(version)
      , sum(sum)
      , stream(stream)
    {}

    static CompiledFile* load(std::istream& stream);
    Object* body(STATE);
    bool execute(STATE);
  };

}

#endif
