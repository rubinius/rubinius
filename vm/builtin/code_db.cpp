#include "builtin/class.hpp"
#include "builtin/code_db.hpp"
#include "builtin/compiled_code.hpp"
#include "builtin/string.hpp"

#include "object_utils.hpp"
#include "ontology.hpp"
#include "marshal.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <string>
#include <fstream>
#include <sstream>

namespace rubinius {
  static CodeDBMap codedb_index;

  void CodeDB::init(STATE) {
    GO(codedb).set(ontology::new_class_under(state, "CodeDB", G(rubinius)));
    G(codedb)->set_object_type(state, CodeDBType);
  }

  CodeDB* CodeDB::open(STATE, String* path) {
    CodeDB* codedb = state->vm()->new_object<CodeDB>(G(codedb));
    codedb->path(state, path->string_dup(state));

    std::string base_path(path->c_str(state));

    std::string data_path = base_path + "/data";
    if((codedb->data_fd_ = ::open(data_path.c_str(), O_RDWR | O_APPEND, 0600)) < 0) {
      Exception::runtime_error(state, "unable to open CodeDB data");
    }

    struct stat st;
    if(stat(data_path.c_str(), &st)) {
      Exception::runtime_error(state, "unable to get CodeDB data size");
    }

    codedb->data_ = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE,
        MAP_PRIVATE, codedb->data_fd_, 0);

    std::string index_path = base_path + "/index";
    std::ifstream stream(index_path.c_str());
    if(!stream) {
      Exception::runtime_error(state, "unable to open CodeDB index");
    }

    while(true) {
      std::string m_id;
      size_t offset, length;

      stream >> m_id;
      stream >> offset;
      stream >> length;
      stream.get();

      if(m_id.empty()) break;

      codedb_index[m_id] = CodeDBIndex(codedb->data_, offset, length);
    }

    return codedb;
  }

  CompiledCode* CodeDB::load(STATE, String* m_id) {
    CodeDBMap::const_iterator index = codedb_index.find(std::string(m_id->c_str(state)));

    if(index == codedb_index.end()) {
      return nil<CompiledCode>();
    }

    CodeDBIndex i = index->second;

    const char* ptr = (const char*)std::get<0>(i);
    std::string data(ptr + std::get<1>(i), std::get<2>(i));
    std::istringstream stream(data);
    UnMarshaller um(state, stream);

    return as<CompiledCode>(um.unmarshal());
  }

  Object* CodeDB::store(STATE, CompiledCode* code) {
    return cNil;
  }
}
