#include "signature.h"

#include "memory.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "ontology.hpp"
#include "marshal.hpp"
#include "metrics.hpp"
#include "thread_phase.hpp"

#include "class/array.hpp"
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
  static CodeDBLoadPathSet codedb_loadpath;
  static CodeDBLoadedFeaturesSet codedb_loadedfeatures;

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

  static bool make_directory(const std::string path) {
    struct stat st;

    if(stat(path.c_str(), &st)) {
      if(mkdir(path.c_str(), S_IRWXU)) return false;
    } else {
      if(!S_ISDIR(st.st_mode)) return false;
    }

    return true;
  }

  static bool file_newer_p(struct stat& src, struct stat& dest) {
#ifdef HAVE_STRUCT_STAT_ST_MTIM
    if(src.st_mtim.tv_sec > dest.st_mtim.tv_sec) return true;
    if(src.st_mtim.tv_nsec > dest.st_mtim.tv_nsec) return true;
#elif HAVE_STRUCT_STAT_ST_MTIMESPEC
    if(src.st_mtimespec.tv_sec > dest.st_mtimespec.tv_sec) return true;
    if(src.st_mtimespec.tv_nsec > dest.st_mtimespec.tv_nsec) return true;
#elif HAVE_STRUCT_STAT_ST_MTIMENSEC
    if(src.st_time > dest.st_mtime) return true;
    if(src.st_mtimensec > dest.st_mtimensec) return true;
#else
    if(src.st_mtime > dest.st_mtime) return true;
#endif

    return false;
  }

#define COPY_FILE_BUFLEN  1048576

  static bool copy_file(const std::string name, const std::string src, const std::string dest) {
    struct stat src_st, dest_st;

    std::string src_path = src + name;
    if(stat(src_path.c_str(), &src_st) || !S_ISREG(src_st.st_mode)) return false;

    std::string dest_path = dest + name;
    if(stat(dest_path.c_str(), &dest_st) == 0 && !S_ISREG(dest_st.st_mode)) return false;

    if(!file_newer_p(src_st, dest_st)) return true;

    int src_fd = ::open(src_path.c_str(), O_RDONLY);
    if(src_fd <= 0) return false;

    int dest_fd = ::open(dest_path.c_str(),
        O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if(dest_fd <= 0) {
      ::close(src_fd);
      return false;
    }

    int num;
    bool status = true;
    char* bytes = new char[COPY_FILE_BUFLEN];

    while((num = ::read(src_fd, bytes, COPY_FILE_BUFLEN)) > 0) {
      if(::write(dest_fd, bytes, num) != num) {
        status = false;
        break;
      }
    }

    ::close(src_fd);
    ::close(dest_fd);

    delete[] bytes;

    return status;
  }

  bool CodeDB::copy_database(STATE, std::string core_path, std::string cache_path) {
    std::string dest_dir = cache_path + "/cache";
    std::string dirs[] = { cache_path, dest_dir };

    for(auto dir : dirs) {
      if(!make_directory(dir)) return false;
    }

    const char* files[] = {
      "/contents", "/data", "/index", "/initialize", "/platform.conf", "/signature"
    };

    std::string src_dir = core_path + "/cache";

    for(auto file : files) {
      if(!copy_file(file, src_dir, dest_dir)) return false;
    }

    return true;
  }

  CodeDB* CodeDB::open(STATE, String* core_path, String* cache_path) {
    return open(state, core_path->c_str(state), cache_path->c_str(state));
  }

  CodeDB* CodeDB::open(STATE, std::string core_path, std::string cache_path) {
    MutexLockWaiting lock_waiting(state, state->shared().codedb_lock());

    CodeDB* codedb = state->memory()->new_object<CodeDB>(state, G(codedb));

    codedb->loaded_features(state, Array::create(state, 0));
    codedb->load_path(state, Array::create(state, 0));

    G(codedb)->set_ivar(state, state->symbol("@current"), codedb);

    std::string base_path;

    CodeDB::copy_database(state, core_path, cache_path);

    if(CodeDB::valid_database_p(state, cache_path)) {
      base_path = cache_path + "/cache";
    } else if(CodeDB::valid_database_p(state, core_path)) {
      base_path = core_path + "/cache";
      codedb->writable(state, cFalse);
    } else {
      Exception::raise_runtime_error(state, "unable to find valid CodeDB");
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

    codedb->size(st.st_size);
    codedb->data(mmap(NULL, state->shared().config.codedb_cache_size.value,
          PROT_READ|PROT_WRITE, MAP_PRIVATE, codedb->data_fd(), 0));

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
      std::string stem, path, feature, m_id;
      uint64_t mtime;

      contents_stream >> stem;
      contents_stream >> path;
      contents_stream >> feature;
      contents_stream >> m_id;
      contents_stream >> mtime;
      contents_stream.get();

      if(stem.empty()) break;

      codedb_contents[stem] = CodeDBContent(path, feature, m_id, mtime, false);
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

  Object* CodeDB::close(STATE) {
    return cNil;
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
    UnMarshaller unmarshaller(state, stream);

    CompiledCode* code = as<CompiledCode>(unmarshaller.unmarshal());

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

  Object* CodeDB::load_feature(STATE,
      String* stem, String* ext, Object* reload, Object* record)
  {
    MutexLockWaiting lock_waiting(state, state->shared().codedb_lock());

    std::string search(stem->c_str(state));
    std::string extstr(ext->c_str(state));

    if(search.rfind(extstr, search.size() - extstr.size()) == std::string::npos) {
      search.append(extstr);
    }

    CodeDBContentMap::iterator index = codedb_contents.find(search);

    if(index != codedb_contents.end()) {
      CodeDBContent& content = index->second;

      if(std::get<4>(content) && reload->false_p()) {
        if(codedb_loadedfeatures.size() != loaded_features()->size()) {
          codedb_loadedfeatures.clear();

          for(native_int i = 0; i < loaded_features()->size(); i++) {
            if(String* feature = try_as<String>(loaded_features()->get(state, i))) {
              codedb_loadedfeatures.insert(feature->c_str(state));
            }
          }
        }

        CodeDBLoadedFeaturesSet::const_iterator lf =
          codedb_loadedfeatures.find(std::get<1>(content));

        if(lf != codedb_loadedfeatures.end()) return cFalse;
      }

      if(CompiledCode* code = try_as<CompiledCode>(
            CodeDB::load(state, std::get<2>(content).c_str())))
      {
        std::string& path = std::get<0>(content);

        if(path.size() == 1 && path[0] == '#') {
          std::ostringstream path;

          path << state->shared().config.codedb_core_path.value
               << "/source/"
               << code->file()->cpp_str(state);

          std::get<1>(content) = path.str();

          code->file(state, state->symbol(path.str().c_str()));
        }

        std::get<4>(content) = true;

        if(record->true_p()) record_feature(state, code->file()->to_str(state));

        logger::debug([&](logger::PrintFunction writer){
            writer("codedb: load path: %s", search.c_str());
          });

        return code;
      }
    }

    return cNil;
  }

  Object* CodeDB::record_feature(STATE, String* feature) {
    loaded_features()->append(state, feature);
    codedb_loadedfeatures.insert(feature->c_str(state));

    return cNil;
  }

  Object* CodeDB::store(STATE, Object* code,
      String* stem, String* path, String* feature, Object* record)
  {
    MutexLockWaiting lock_waiting(state, state->shared().codedb_lock());

    const char* path_str = path->byte_size() == 0 ? "#" : path->c_str(state);

    codedb_contents[stem->c_str(state)] =
        CodeDBContent(path_str, feature->c_str(state), "", 0, true);

    if(record->true_p()) record_feature(state, feature);

    logger::debug([&](logger::PrintFunction writer){
        writer("codedb: store: %d %s %s %s", writable()->true_p(),
            stem->c_str(state), path_str, feature->c_str(state));
      });

    if(writable()->false_p()) return code;

    if(CompiledCode* ccode = try_as<CompiledCode>(code)) {
      std::string stream_data(static_cast<char*>(data()) + size());
      std::ostringstream stream(stream_data);
      Marshaller marshaller(state, ccode, stream);

      marshaller.marshal();

      ccode->stamp_id(state);

      codedb_index[ccode->code_id()->c_str(state)] =
          CodeDBIndex(data(), size(), stream_data.size());

      size(size() + stream_data.size());
    }

    return code;
  }
}
