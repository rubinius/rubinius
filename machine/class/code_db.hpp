#ifndef RBX_CODEDB_HPP
#define RBX_CODEDB_HPP

#include "class/object.hpp"

#include "missing/unordered_map.hpp"

#include <tuple>

namespace rubinius {
  class CompiledCode;

  typedef std::tuple<void*, size_t, size_t> CodeDBIndex;
  typedef std_unordered_map<std::string, CodeDBIndex> CodeDBMap;

  class CodeDB : public Object {
  public:
    const static object_type type = CodeDBType;

    attr_accessor(path, String);

  private:
    attr_field(data_fd, int);
    attr_field(data, void*);

  public:
    static void bootstrap(STATE);
    static bool valid_database_p(STATE, std::string path);

    // Rubinius.primitive :code_db_open
    static CodeDB* open(STATE, String* path);
    static CodeDB* open(STATE, const char* path);

    // Rubinius.primitive :code_db_load
    static CompiledCode* load(STATE, String* m_id);
    static CompiledCode* load(STATE, const char* m_id);

    static CompiledCode* load(STATE, Object* id_or_code);

    // Rubinius.primitive :code_db_store
    Object* store(STATE, CompiledCode* code);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
