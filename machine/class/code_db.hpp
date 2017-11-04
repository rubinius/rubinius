#ifndef RBX_CODEDB_HPP
#define RBX_CODEDB_HPP

#include "class/object.hpp"

#include <tuple>
#include <unordered_map>
#include <unordered_set>

namespace rubinius {
  class Array;
  class CompiledCode;
  class String;

  typedef std::tuple<void*, size_t, size_t> CodeDBIndex;
  typedef std::tuple<std::string, std::string, std::string, uint64_t, bool> CodeDBContent;
  typedef std::unordered_map<std::string, CodeDBIndex> CodeDBMap;
  typedef std::unordered_map<std::string, CodeDBContent> CodeDBContentMap;
  typedef std::unordered_set<std::string> CodeDBLoadPathSet;
  typedef std::unordered_set<std::string> CodeDBLoadedFeaturesSet;

  class CodeDB : public Object {
  public:
    const static object_type type = CodeDBType;

    attr_accessor(loaded_features, Array);
    attr_accessor(load_path, Array);
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

    // Rubinius.primitive :code_db_load_feature
    Object* load_feature(STATE, String* stem, String* ext, Object* reload, Object* record);

    // Rubinius.primitive :code_db_record_feature
    Object* record_feature(STATE, String* feature);

    // Rubinius.primitive :code_db_store
    Object* store(STATE, Object* code, String* stem, String* path, String* feature, Object* record);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
