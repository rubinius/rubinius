#ifndef RBX_COMPILED_FILE_HPP
#define RBX_COMPILED_FILE_HPP

#include "builtin.hpp"

namespace rubinius {
  class CompiledFile {
  public:
    std::string magic;
    long version;
    std::string sum;

  private:
    std::istream* stream;

  public:
    CompiledFile(std::string magic, long version, std::string sum, 
        std::istream* stream) : 
          magic(magic), version(version), sum(sum), 
          stream(stream) { }


    static CompiledFile* load(std::istream& stream);
    OBJECT body(STATE);
  };
}

#endif
