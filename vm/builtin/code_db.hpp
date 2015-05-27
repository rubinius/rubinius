#ifndef RBX_CODEDB_HPP
#define RBX_CODEDB_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class CompiledCode;

  class CodeDB : public Object {
  public:
    const static object_type type = CodeDBType;

  private:
    String* path_;      // slot
    int data_fd_;

  public:
    attr_accessor(path, String);

    static void init(STATE);
    static CodeDB* create(STATE, const char* path);

    // Rubinius.primitive :code_db_open
    static CodeDB* open(STATE, String* path);

    // Rubinius.primitive :code_db_load
    static CompiledCode* load(STATE, String* m_id);

    // Rubinius.primitive :code_db_store
    Object* store(STATE, CompiledCode* code);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
