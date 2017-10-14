#include "signature.h"

#include "memory.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "ontology.hpp"
#include "marshal.hpp"
#include "metrics.hpp"
#include "thread_phase.hpp"

#include "class/class.hpp"
#include "class/code_db.hpp"
#include "class/compiled_code.hpp"
#include "class/string.hpp"

#include "instruments/timing.hpp"

#include "util/thread.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <string>
#include <fstream>
#include <sstream>

namespace rubinius {
  static CodeDBMap codedb_index;
  static CodeDBContentMap codedb_contents;

  void CodeDB::bootstrap(STATE) {
    GO(codedb).set(state->memory()->new_class<Class, CodeDB>(
          state, G(rubinius), "CodeDB"));
    G(codedb)->set_object_type(state, CodeDBType);
  }

  bool CodeDB::valid_database_p(STATE, std::string path) {
    std::string cache_path(path + "/cache");
    struct stat st;

    if(stat(cache_path.c_str(), &st) || !S_ISDIR(st.st_mode)) {
      return false;
    }

    std::string signature_path = cache_path + "/signature";
    if(stat(signature_path.c_str(), &st) || !S_ISREG(st.st_mode)) {
      return false;
    }

    std::ifstream signature(signature_path.c_str());
    if(signature) {
      uint64_t sig;
      signature >> sig;
      signature.close();

      if(sig != RBX_SIGNATURE) return false;
    } else {
      return false;
    }

    std::string index_path = cache_path + "/index";
    if(stat(index_path.c_str(), &st) || !S_ISREG(st.st_mode)) {
      return false;
    }

    std::string contents_path = cache_path + "/contents";
    if(stat(contents_path.c_str(), &st) || !S_ISREG(st.st_mode)) {
      return false;
    }

    std::string data_path = cache_path + "/data";
    if(stat(data_path.c_str(), &st) || !S_ISREG(st.st_mode)) {
      return false;
    }

    return true;
  }

  bool CodeDB::copy_database(STATE, std::string core_path, std::string cache_path) {
    return false;
  }

  CodeDB* CodeDB::open(STATE, String* core_path, String* cache_path) {
    return open(state, core_path->c_str(state), cache_path->c_str(state));
  }

  CodeDB* CodeDB::open(STATE, std::string core_path, std::string cache_path) {
    MutexLockWaiting lock_waiting(state, state->shared().codedb_lock());

    CodeDB* codedb = state->memory()->new_object<CodeDB>(state, G(codedb));

    std::string base_path;

    if(CodeDB::valid_database_p(state, cache_path)) {
      base_path = cache_path;
    } else {
      if(CodeDB::valid_database_p(state, core_path)) {
        if(CodeDB::copy_database(state, core_path, cache_path)) {
          base_path = cache_path + "/cache";
        } else {
          base_path = core_path + "/cache";
          codedb->writable(state, cFalse);
        }
      } else {
        Exception::raise_runtime_error(state, "unable to find valid CodeDB");
      }
    }

    codedb->path(state, String::create(state, base_path.c_str()));

    logger::write("codedb: loading: %s", base_path.c_str());

    // Map the CodeDB data to memory.
    std::string data_path = base_path + "/data";
    codedb->data_fd(::open(data_path.c_str(), O_RDONLY));

    if(codedb->data_fd() < 0) {
      Exception::raise_runtime_error(state, "unable to open CodeDB data");
    }

    struct stat st;
    if(stat(data_path.c_str(), &st)) {
      Exception::raise_runtime_error(state, "unable to get CodeDB data size");
    }

    codedb->data(mmap(NULL, st.st_size, PROT_READ,
        MAP_PRIVATE, codedb->data_fd(), 0));

    // Read the method id index.
    std::string index_path = base_path + "/index";
    std::ifstream data_stream(index_path.c_str());
    if(!data_stream) {
      Exception::raise_runtime_error(state, "unable to open CodeDB index");
    }

    while(true) {
      std::string m_id;
      size_t offset, length;

      data_stream >> m_id;
      data_stream >> offset;
      data_stream >> length;
      data_stream.get();

      if(m_id.empty()) break;

      codedb_index[m_id] = CodeDBIndex(codedb->data(), offset, length);
    }
    data_stream.close();

    // Read the contents.
    std::string contents_path = base_path + "/contents";
    std::ifstream contents_stream(contents_path);
    if(!contents_stream) {
      Exception::raise_runtime_error(state, "unable to open CodeDB contents index");
    }

    while(true) {
      std::string name, path, m_id;
      uint64_t mtime;

      contents_stream >> name;
      contents_stream >> path;
      contents_stream >> m_id;
      contents_stream >> mtime;
      contents_stream.get();

      if(name.empty()) break;

      codedb_contents[name] = CodeDBContent(path, m_id, mtime, false);
    }
    contents_stream.close();

    // Run all initial methods.
    std::string initialize_path = base_path + "/initialize";
    std::ifstream initialize_stream(initialize_path.c_str());
    if(!initialize_stream) {
      Exception::raise_runtime_error(state, "unable to open CodeDB initialize");
    }

    while(true) {
      std::string m_id;

      initialize_stream >> m_id;
      if(m_id.empty()) break;

      CompiledCode* code = load(state, m_id.c_str());
      OnStack<1> os(state, code);

      if(code->nil_p()) {
        Exception::raise_runtime_error(state, "unable to resolve method in CodeDB initialize");
      }

      code->execute_script(state);
    }

    return codedb;
  }

  CompiledCode* CodeDB::load(STATE, const char* m_id) {
    timer::StopWatch<timer::nanoseconds> timer(
        state->shared().codedb_metrics().load_ns);

    state->shared().codedb_metrics().load_count++;

    MutexLockWaiting lock_waiting(state, state->shared().codedb_lock());

    CodeDBMap::const_iterator index = codedb_index.find(std::string(m_id));

    if(index == codedb_index.end()) {
      return nil<CompiledCode>();
    }

    CodeDBIndex i = index->second;

    const char* ptr = (const char*)std::get<0>(i);
    std::string data(ptr + std::get<1>(i), std::get<2>(i));
    std::istringstream stream(data);
    UnMarshaller um(state, stream);

    CompiledCode* code = as<CompiledCode>(um.unmarshal());

    logger::debug([&](logger::PrintFunction writer){
        writer("codedb: load: %s, %s",
            code->file()->cpp_str(state).c_str(),
            code->name()->cpp_str(state).c_str());
      });

    return code;
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

  Object* CodeDB::load_path(STATE, String* path, String* ext) {
    MutexLockWaiting lock_waiting(state, state->shared().codedb_lock());

    std::string search(path->c_str(state));
    std::string extstr(ext->c_str(state));

    if(search.rfind(extstr, search.size() - extstr.size()) == std::string::npos) {
      search.append(extstr);
    }

    CodeDBContentMap::iterator index = codedb_contents.find(search);

    if(index != codedb_contents.end()) {
      CodeDBContent& content = index->second;

      if(std::get<3>(content)) return cFalse;

      if(CompiledCode* code = CodeDB::load(state, std::get<1>(content).c_str())) {
        std::string& path = std::get<0>(content);

        if(path.size() == 1 && path[0] == '.') {
          std::ostringstream path;

          path << state->shared().config.codedb_core_path.value
               << "/source/"
               << code->file()->cpp_str(state);

          code->file(state, state->symbol(path.str().c_str()));
        }

        std::get<3>(content) = true;

        logger::debug([&](logger::PrintFunction writer){
            writer("codedb: load path: %s", search.c_str());
          });

        return code;
      }
    }

    return cNil;
  }

  Object* CodeDB::store(STATE, CompiledCode* code) {
    return cNil;
  }
}
