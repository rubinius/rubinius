#include "signature.h"

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
    return open(state, path->c_str(state));
  }

  CodeDB* CodeDB::open(STATE, const char* path) {
    CodeDB* codedb = state->vm()->new_object<CodeDB>(G(codedb));
    codedb->path(state, String::create(state, path));

    std::string base_path(path);

    // Check the CodeDB signature matches the VM.
    std::string signature_path = base_path + "/signature";
    std::ifstream signature(signature_path.c_str());
    if(signature) {
      uint64_t sig;
      signature >> sig;

      if(sig != RBX_SIGNATURE) {
        Exception::runtime_error(state,
            "the CodeDB signature is not valid for this version");
      }

      signature.close();
    } else {
      Exception::runtime_error(state, "unable to open CodeDB signature");
    }

    // Map the CodeDB data to memory.
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

    // Read the method id index.
    std::string index_path = base_path + "/index";
    std::ifstream data_stream(index_path.c_str());
    if(!data_stream) {
      Exception::runtime_error(state, "unable to open CodeDB index");
    }

    while(true) {
      std::string m_id;
      size_t offset, length;

      data_stream >> m_id;
      data_stream >> offset;
      data_stream >> length;
      data_stream.get();

      if(m_id.empty()) break;

      codedb_index[m_id] = CodeDBIndex(codedb->data_, offset, length);
    }
    data_stream.close();

    // Run all initial methods.
    std::string initialize_path = base_path + "/initialize";
    std::ifstream initialize_stream(initialize_path.c_str());
    if(!initialize_stream) {
      Exception::runtime_error(state, "unable to open CodeDB initialize");
    }

    while(true) {
      std::string m_id;

      initialize_stream >> m_id;
      if(m_id.empty()) break;

      CompiledCode* code = load(state, m_id.c_str());
      if(code->nil_p()) {
        Exception::runtime_error(state, "unable to resolve method in CodeDB initialize");
      }

      code->execute_script(state);
    }

    return codedb;
  }

  CompiledCode* CodeDB::load(STATE, const char* m_id) {
    CodeDBMap::const_iterator index = codedb_index.find(std::string(m_id));

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

  CompiledCode* CodeDB::load(STATE, String* m_id) {
    return load(state, m_id->c_str(state));
  }

  CompiledCode* CodeDB::load(STATE, Object* id_or_code) {
    if(String* m_id = try_as<String>(id_or_code)) {
      return CodeDB::load(state, m_id);
    }

    return as<CompiledCode>(id_or_code);
  }

  Object* CodeDB::store(STATE, CompiledCode* code) {
    return cNil;
  }
}
