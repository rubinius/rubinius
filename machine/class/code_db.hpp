#ifndef RBX_CODEDB_HPP
#define RBX_CODEDB_HPP

#include "class/object.hpp"

#include "signature.h"

#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

namespace rubinius {
  class Array;
  class CompiledCode;
  class String;

  typedef std::tuple<size_t, size_t> CodeDBIndex;
  typedef std::pair<std::string, CodeDBIndex> CodeDBIndexPair;
  typedef std::unordered_map<std::string, CodeDBIndex> CodeDBMap;
  typedef std::tuple<std::string, std::string, std::string, uint64_t, bool> CodeDBContent;
  typedef std::pair<std::string, CodeDBContent> CodeDBContentPair;
  typedef std::unordered_map<std::string, CodeDBContent> CodeDBContentMap;
  typedef std::unordered_set<std::string> CodeDBLoadPathSet;
  typedef std::unordered_set<std::string> CodeDBLoadedFeatureSet;

  class CodeDB : public Object {
  public:
    const static object_type type = CodeDBType;

    const static int header_size = 156;
    const static int version = 1;
    const static char* magic;
    const static char* signature;

    struct Region {
      int begin;
      int end;

      void set(int start, int size) {
        begin = start;
        end = start + size;
      }
    };

    struct Regions {
      Region initialize;
      Region data;
      Region index;
      Region contents;
    };

  public:
    attr_accessor(loaded_features, Array);
    attr_accessor(load_path, Array);
    attr_accessor(path, String);
    attr_accessor(writable, Object);

  private:
    attr_field(data_fd, int);
    attr_field(mptr, void*);
    attr_field(data, char*);
    attr_field(size, size_t);
    attr_field(regions, Regions*);
    attr_field(index, CodeDBMap*);
    attr_field(contents, CodeDBContentMap*);
    attr_field(path_set, CodeDBLoadPathSet*);
    attr_field(feature_set, CodeDBLoadedFeatureSet*);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, CodeDB* codedb) {
      codedb->path(nil<String>());
      codedb->writable(cFalse);
    }

    static bool valid_database_p(STATE, std::string path);
    static bool copy_database(STATE, std::string src, std::string dest);
    static void purge(STATE, std::string path);

    // Rubinius.primitive :code_db_open
    static CodeDB* open(STATE, String* path);

    static CodeDB* open(STATE, std::string path, bool writable=false);
    static CodeDB* open(STATE, std::string core_path, std::string cache_path);

    // Rubinius.primitive :code_db_load
    CompiledCode* load(STATE, String* c_id);
    CompiledCode* load(STATE, const char* c_id);

    CompiledCode* load(STATE, Object* id_or_code);

    // Rubinius.primitive :code_db_close
    Object* close(STATE);

    // Rubinius.primitive :code_db_load_db
    Object* load_db(STATE);

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
