#ifndef RBX_CODEDB_HPP
#define RBX_CODEDB_HPP

#include "builtin/object.hpp"

#include "missing/unordered_map.hpp"

#include <tuple>

namespace rubinius {
  class CompiledCode;

  typedef std::tuple<void*, size_t, size_t> CodeDBIndex;
  typedef std_unordered_map<std::string, CodeDBIndex> CodeDBMap;

  class CodeDB : public Object {
  public:
    const static object_type type = CodeDBType;

  private:
    String* path_;      // slot
    int data_fd_;
    void* data_;

  public:
    attr_accessor(path, String);

    static void init(STATE);

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
