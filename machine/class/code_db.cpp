#include "memory.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "ontology.hpp"
#include "marshal.hpp"
#include "thread_phase.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/code_db.hpp"
#include "class/compiled_code.hpp"
#include "class/string.hpp"

#include "diagnostics/codedb.hpp"
#include "diagnostics/timing.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stack>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>

namespace rubinius {
  const char* CodeDB::magic = "!RBIX";
  const char* CodeDB::signature = RBX_SIGNATURE;

  void CodeDB::bootstrap(STATE) {
    GO(codedb).set(state->memory()->new_class<Class, CodeDB>(
          state, G(rubinius), "CodeDB"));
    G(codedb)->set_object_type(state, CodeDBType);
  }

  void CodeDB::purge(STATE, std::string path) {
    (void)::unlink(path.c_str());
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

  bool CodeDB::copy_database(STATE, std::string src, std::string dest) {
    struct stat src_st, dest_st;

    if(stat(src.c_str(), &src_st) || !S_ISREG(src_st.st_mode)) return false;
    if(stat(dest.c_str(), &dest_st) == 0 && !S_ISREG(dest_st.st_mode)) return false;

    if(!file_newer_p(src_st, dest_st)) return true;

    logger::write("codedb: copy: %s, %s", src.c_str(), dest.c_str());

    int src_fd = ::open(src.c_str(), O_RDONLY);

    if(src_fd < 0) return false;

    int dest_fd = ::open(dest.c_str(),
        O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

    if(dest_fd < 0) {
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

  CodeDB* CodeDB::open(STATE, String* path) {
    return CodeDB::open(state, path->c_str(state));
  }

  CodeDB* CodeDB::open(STATE, std::string db_path, bool writable) {
    memory::Collector::Inhibit inhibitor(state);
    std::lock_guard<std::recursive_mutex> lock(state->memory()->codedb_lock());

    CodeDB* codedb = state->memory()->new_object<CodeDB>(state, G(codedb));

    codedb->loaded_features(state, Array::create(state, 0));
    codedb->load_path(state, Array::create(state, 0));

    codedb->path_set(new CodeDBLoadPathSet);
    codedb->feature_set(new CodeDBLoadedFeatureSet);

    codedb->path(state, String::create(state, db_path.c_str()));

    std::ifstream stream(db_path);
    if(!stream.is_open()) return nullptr;

    codedb->regions(new Regions);

    std::string magic;
    std::string signature;
    int version;

    stream >> magic;
    stream >> version;
    stream >> signature;

    if(state->configuration()->codedb_cache_validate) {
      if(!magic.compare(CodeDB::magic)) return nullptr;
      if(version != CodeDB::version) return nullptr;
      if(!signature.compare(CodeDB::signature)) return nullptr;
    }

    int region_size;

    stream >> region_size;
    codedb->regions()->initialize.set(CodeDB::header_size, region_size);

    stream >> region_size;
    stream >> region_size;
    codedb->regions()->data.set(codedb->regions()->initialize.end, region_size);

    stream >> region_size;
    codedb->regions()->index.set(codedb->regions()->data.end, region_size);

    stream >> region_size;
    codedb->regions()->contents.set(codedb->regions()->index.end, region_size);

    // Memory map data
    codedb->data_fd(::open(db_path.c_str(), writable ? O_RDWR : O_RDONLY));

    if(codedb->data_fd() < 0) return nullptr;

    codedb->mptr(mmap(NULL, state->configuration()->codedb_cache_size,
          PROT_READ|PROT_WRITE, MAP_PRIVATE, codedb->data_fd(), 0));

    codedb->data(static_cast<char*>(codedb->mptr()) + codedb->regions()->data.begin);
    codedb->size(codedb->regions()->data.end - codedb->regions()->data.begin);

    // Read the code ID index.
    codedb->index(new CodeDBMap);
    stream.seekg(codedb->regions()->index.begin, stream.beg);

    while(true) {
      if(stream.tellg() >= codedb->regions()->index.end) break;

      std::string c_id;
      size_t offset, length;

      stream >> c_id;
      stream >> offset;
      stream >> length;
      stream.get();

      codedb->index()->insert(
          CodeDBIndexPair(std::move(c_id),
            CodeDBIndex(std::move(offset), std::move(length))));
    }

    // Read the contents.
    codedb->contents(new CodeDBContentMap);
    stream.seekg(codedb->regions()->contents.begin, stream.beg);

    while(true) {
      if(stream.tellg() >= codedb->regions()->contents.end) break;

      std::string stem, path, feature, c_id;
      uint64_t mtime;
      int name_size;

      stream >> name_size;
      if(name_size > 0) stream >> stem;

      stream >> name_size;
      if(name_size > 0) stream >> path;

      stream >> name_size;
      if(name_size > 0) stream >> feature;

      stream >> c_id;
      stream >> mtime;
      stream.get();

      codedb->contents()->insert(
          CodeDBContentPair(std::move(stem),
            CodeDBContent(std::move(path), std::move(feature), std::move(c_id), mtime, false)));
    }

    stream.close();

    return codedb;
  }

  CodeDB* CodeDB::open(STATE, std::string core_path, std::string cache_path) {
    CodeDB* codedb = 0;

    std::string base_path;

    core_path.append("/cache");

    if(state->configuration()->codedb_cache_purge) {
      CodeDB::purge(state, cache_path);
    }

    if(state->configuration()->codedb_cache_enabled) {
      if(!CodeDB::copy_database(state, core_path, cache_path)) {
        logger::write("codedb: copy failed: %s, %s",
            core_path.c_str(), cache_path.c_str());
      }
    }

    if(state->configuration()->codedb_cache_enabled
        && (codedb = CodeDB::open(state, cache_path))) {
      base_path = cache_path;
      codedb->writable(state, cFalse);
    } else if((codedb = CodeDB::open(state, core_path))) {
      base_path = core_path;
      codedb->writable(state, cFalse);
    } else {
      Exception::raise_runtime_error(state, "unable to find valid CodeDB");
    }

    logger::write("codedb: loading: %s", base_path.c_str());

    GO(coredb).set(codedb);
    G(rubinius)->set_const(state, "CoreDB", codedb);

    codedb->load_db(state);

    return codedb;
  }

  Object* CodeDB::load_db(STATE) {
    std::ifstream stream(path()->c_str(state));

    // Run all initial methods.
    stream.seekg(regions()->initialize.begin, stream.beg);

    while(true) {
      if(stream.tellg() >= regions()->initialize.end) break;

      std::string c_id;

      stream >> c_id;
      stream.get();

      CompiledCode* code = load(state, c_id.c_str());
      OnStack<1> os(state, code);

      if(code->nil_p()) {
        Exception::raise_runtime_error(state,
            "unable to resolve method in CodeDB initialize");
      }

      code->execute_script(state);
    }

    stream.close();

    return cNil;
  }

#define CODEDB_WRITE_FLAGS  std::ofstream::out \
                            | std::ofstream::trunc \
                            | std::ofstream::ate \
                            | std::ofstream::binary

  Object* CodeDB::close(STATE) {
    ::munmap(data(), state->configuration()->codedb_cache_size);
    ::close(data_fd());

    delete index();
    index(nullptr);

    delete contents();
    contents(nullptr);

    delete path_set();
    path_set(nullptr);

    delete feature_set();
    feature_set(nullptr);

    delete regions();
    regions(nullptr);

    // TODO: enable write
    return cNil;

    off_t index_offset;

    // Write the data.
    size_t update_size;

    if((update_size = size() - regions()->data.end) > 0) {
      if(::lseek(data_fd(), regions()->data.end, SEEK_SET) < 0) {
        logger::warn("codedb: close: %s: failed to write data", strerror(errno));
      }

      const char* ptr = static_cast<const char*>(data())
        + (regions()->data.end - regions()->data.begin);
      if(::write(data_fd(), ptr, update_size) != update_size) {
        logger::warn("codedb: close: %s: failed to write data", strerror(errno));
      } else {
        index_offset = ::lseek(data_fd(), 0, SEEK_CUR);
      }
    } else {
      index_offset = regions()->data.end;
    }

    ::munmap(data(), state->configuration()->codedb_cache_size);
    ::close(data_fd());

    std::ofstream stream;
    stream.open(path()->c_str(state), CODEDB_WRITE_FLAGS);

    if(!stream.is_open()) {
      logger::warn("codedb: close: %s: failed to open cache", strerror(errno));
    }

    // Write the code ID index.
    stream.seekp(index_offset, stream.beg);

    for(CodeDBMap::const_iterator i = index()->begin();
        i != index()->end();
        ++i)
    {
      const std::string& c_id = i->first;
      const CodeDBIndex& index = i->second;

      stream << c_id << " "
             << std::dec << std::get<0>(index) << " "
             << std::dec << std::get<1>(index) << "\n";
    }

    // Write the contents.
    for(CodeDBContentMap::const_iterator i = contents()->begin();
        i != contents()->end();
        ++i)
    {
      const std::string& stem = i->first;
      const CodeDBContent& contents = i->second;

      const std::string& path = std::get<0>(contents);
      const std::string& feature = std::get<1>(contents);
      const std::string& c_id = std::get<2>(contents);

      stream << stem.size() << " ";
      if(stem.size() > 0) stream << stem << " ";

      stream << path.size() << " ";
      if(path.size() > 0) stream << path << " ";

      stream << feature.size() << " ";
      if(feature.size() > 0) stream << feature << " ";

      if(c_id.size() > 0) {
        stream << c_id << " ";
      } else {
        stream << "0" << " ";
      }

      stream << std::get<3>(contents) << "\n";
    }

    stream.close();

    return cNil;
  }

  CompiledCode* CodeDB::load(STATE, const char* c_id) {
    memory::Collector::Inhibit inhibitor(state);
    std::lock_guard<std::recursive_mutex> lock(state->memory()->codedb_lock());

    timer::StopWatch<timer::nanoseconds> timer(
        state->machine()->diagnostics()->codedb_metrics()->load_ns);

    state->machine()->diagnostics()->codedb_metrics()->load_count++;

    CodeDBMap::const_iterator m = index()->find(std::string(c_id));

    if(m == index()->end()) {
      return nil<CompiledCode>();
    }

    CodeDBIndex i = m->second;

    const char* ptr = static_cast<const char*>(data());
    std::string data(ptr + std::get<0>(i), std::get<1>(i));

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

  CompiledCode* CodeDB::load(STATE, String* c_id) {
    return load(state, c_id->c_str(state));
  }

  CompiledCode* CodeDB::load(STATE, Object* id_or_code) {
    if(String* c_id = try_as<String>(id_or_code)) {
      return load(state, c_id);
    }

    return as<CompiledCode>(id_or_code);
  }

  Object* CodeDB::load_feature(STATE,
      String* stem, String* ext, Object* reload, Object* record)
  {
    memory::Collector::Inhibit inhibitor(state);
    std::lock_guard<std::recursive_mutex> lock(state->memory()->codedb_lock());

    std::string search(stem->c_str(state));
    std::string extstr(ext->c_str(state));

    if(search.rfind(extstr, search.size() - extstr.size()) == std::string::npos) {
      search.append(extstr);
    }

    CodeDBContentMap::iterator index = contents()->find(search);

    if(index != contents()->end()) {
      CodeDBContent& content = index->second;

      if(std::get<4>(content) && reload->false_p()) {
        if(feature_set()->size() != loaded_features()->size()) {
          feature_set()->clear();

          for(intptr_t i = 0; i < loaded_features()->size(); i++) {
            if(String* feature = try_as<String>(loaded_features()->get(state, i))) {
              feature_set()->insert(feature->c_str(state));
            }
          }
        }

        CodeDBLoadedFeatureSet::const_iterator lf =
          feature_set()->find(std::get<1>(content));

        if(lf != feature_set()->end()) return cFalse;
      }

      if(CompiledCode* code = try_as<CompiledCode>(
            load(state, std::get<2>(content).c_str())))
      {
        std::string& path = std::get<0>(content);

        if(path.empty()) {
          std::ostringstream path;

          path << state->configuration()->codedb_core_path.value
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
    feature_set()->insert(feature->c_str(state));

    return cNil;
  }

  Object* CodeDB::store(STATE, Object* code,
      String* stem, String* path, String* feature, Object* record)
  {
    memory::Collector::Inhibit inhibitor(state);
    std::lock_guard<std::recursive_mutex> lock(state->memory()->codedb_lock());

    const char* c_id;

    if(false) { /*CompiledCode* ccode = try_as<CompiledCode>(code)) {
      BlockPhase blocking(state);

      std::stack<CompiledCode*> code_stack;

      code_stack.push(ccode);

      while(!code_stack.empty()) {
        ccode = code_stack.top();

        Tuple* literals = ccode->literals();

        for(int i = 0; i < literals->num_fields(); i++) {
          if(CompiledCode* c = try_as<CompiledCode>(literals->at(i))) {
            c->stamp_id(state);
            literals->put(state, i, c->code_id());
            code_stack.push(c);
          }
        }

        std::ostringstream stream;
        Marshaller marshaller(state, ccode, stream);

        marshaller.marshal();

        std::string data_str = stream.str();

        ccode->stamp_id(state);
        c_id = ccode->code_id()->c_str(state);

        (*index())[c_id] = CodeDBIndex(size(), data_str.size());

        size(size() + data_str.size());

        code_stack.pop();
      }
      */
    } else {
      c_id = "";
    }

    contents()->insert(CodeDBContentPair(stem->c_str(state),
        CodeDBContent(path->c_str(state), feature->c_str(state), c_id, 0, true)));

    if(record->true_p() && feature->byte_size() > 0) {
      record_feature(state, feature);
    }

    logger::debug([&](logger::PrintFunction writer){
        writer("codedb: store: %s %s", stem->c_str(state), feature->c_str(state));
      });

    return code;
  }
}
