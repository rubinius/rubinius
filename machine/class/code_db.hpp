#ifndef RBX_CODEDB_HPP
#define RBX_CODEDB_HPP

#include "class/object.hpp"

#include <tuple>
#include <unordered_map>

namespace rubinius {
  class CompiledCode;

  typedef std::tuple<void*, size_t, size_t> CodeDBIndex;
  typedef std::unordered_map<std::string, CodeDBIndex> CodeDBMap;
  typedef std::unordered_map<std::string, std::string> CodeDBContents;

  class CodeDB : public Object {
  public:
    const static object_type type = CodeDBType;

    attr_accessor(path, String);
    attr_accessor(writable, Object);

  private:
    attr_field(data_fd, int);
    attr_field(data, void*);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, CodeDB* codedb) {
      codedb->path(nil<String>());
      codedb->writable(cFalse);
    }

    static bool valid_database_p(STATE, std::string path);
    static bool copy_database(STATE, std::string core_path, std::string cache_path);

    // Rubinius.primitive :code_db_open
    static CodeDB* open(STATE, String* core_path, String* cache_path);
    static CodeDB* open(STATE, std::string core_path, std::string cache_path);

    // Rubinius.primitive :code_db_load
    static CompiledCode* load(STATE, String* m_id);
    static CompiledCode* load(STATE, const char* m_id);

    static CompiledCode* load(STATE, Object* id_or_code);

    // Rubinius.primitive :code_db_load_path
    Object* load_path(STATE, String* path);

    // Rubinius.primitive :code_db_store
    Object* store(STATE, CompiledCode* code);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
