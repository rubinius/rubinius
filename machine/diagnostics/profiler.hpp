#ifndef RBX_DIAGNOSTICS_PROFILER_HPP
#define RBX_DIAGNOSTICS_PROFILER_HPP

#include "diagnostics/diagnostic.hpp"

#include <string>
#include <unordered_map>

namespace rubinius {
  namespace diagnostics {
    class ProfilerEntryKey {
    public:
      uint64_t serial;
      int ip;

      ProfilerEntryKey(uint64_t s, int i)
        : serial(s)
        , ip(i)
      {
      }
    };

    bool operator==(const ProfilerEntryKey& a, const ProfilerEntryKey& b);
  }
}

namespace std {
  template <>
    struct hash<rubinius::diagnostics::ProfilerEntryKey> {
      using argument_type = rubinius::diagnostics::ProfilerEntryKey;

      size_t operator()(const argument_type& key) const {
        return hash<uint64_t>()(key.serial) ^ hash<int>()(key.ip);
      }
    };
}

namespace rubinius {
  namespace diagnostics {
    class Profiler : public Diagnostic {
    public:

      class IndexEntry {
      public:
        std::string name;
        std::string location;
        uint64_t samples;
        uint64_t invokes;

        IndexEntry(const std::string& n, const std::string& l, uint64_t s, uint64_t i)
          : name(n)
          , location(l)
          , samples(s)
          , invokes(i)
        {
        }
      };

      class Entry {
      public:
        uint64_t serial;
        int cache_hits;
        std::string receiver_class;
        std::string module_name;

        Entry(uint64_t s, int h, const std::string& r, const std::string& m)
          : serial(s)
          , cache_hits(h)
          , receiver_class(r)
          , module_name(m)
        {
        }
      };

      std::unordered_map<uint64_t, IndexEntry> index_;
      std::unordered_map<ProfilerEntryKey, Entry> entries_;

      int sample_min_;
      bool collecting_;

    public:
      Profiler()
        : Diagnostic()
        , index_()
        , entries_()
        , sample_min_(10)
        , collecting_(false)
      {
        set_type("Profiler");

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("index", rapidjson::Value(rapidjson::kArrayType).Move(), alloc);
        document_.AddMember("entries", rapidjson::Value(rapidjson::kArrayType).Move(), alloc);
      }

      virtual ~Profiler() { }

      int sample_min() const {
        return sample_min_;
      }

      virtual void start_reporting(STATE) {
        if(state->shared().config.diagnostics_profiler_enabled) {
          Diagnostic::start_reporting(state);
          collecting_ = true;
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->shared().config.diagnostics_profiler_enabled) {
          Diagnostic::stop_reporting(state);
          collecting_ = false;
        }
      }

      virtual const std::string to_string() {
        rapidjson::Value& index = document_["index"];
        index.Clear();

        rapidjson::Value& entries = document_["entries"];
        entries.Clear();

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        for(auto i : index_) {
          const IndexEntry& e = i.second;

          rapidjson::Value o(rapidjson::kObjectType);

          o.AddMember("serial", i.first, alloc);
          o.AddMember("name", rapidjson::Value(e.name.c_str(), alloc).Move(), alloc);
          o.AddMember("location", rapidjson::Value(e.location.c_str(), alloc).Move(), alloc);
          o.AddMember("samples", e.samples, alloc);
          o.AddMember("calls", e.invokes, alloc);

          index.PushBack(o.Move(), alloc);
        }

        for(auto e : entries_) {
          const ProfilerEntryKey& k = e.first;
          const Entry& v = e.second;

          rapidjson::Value o(rapidjson::kObjectType);

          o.AddMember("caller_serial", k.serial, alloc);
          o.AddMember("ip", k.ip, alloc);
          o.AddMember("callee_serial", v.serial, alloc);
          o.AddMember("cache_hits", v.cache_hits, alloc);
          o.AddMember("class",
              rapidjson::Value(v.receiver_class.c_str(), alloc).Move(), alloc);
          o.AddMember("module",
              rapidjson::Value(v.module_name.c_str(), alloc).Move(), alloc);

          entries.PushBack(o.Move(), alloc);
        }

        const std::string& str = Diagnostic::to_string();

        index.Clear();
        entries.Clear();

        return std::move(str);
      }

      bool collecting_p() const {
        return collecting_;
      }

      virtual void add_index(uint64_t serial, const std::string& name,
          const std::string& location, uint64_t samples, uint64_t invokes)
      {
        index_.insert(std::make_pair(serial,
              IndexEntry(name, location, samples, invokes)));
      }

      virtual void add_entry(uint64_t caller_serial, int ip, uint64_t callee_serial,
          int hits, const std::string& receiver_class, const std::string& module_name)
      {
        entries_.insert(std::make_pair(ProfilerEntryKey(caller_serial, ip),
              Entry(callee_serial, hits, receiver_class, module_name)));
      }
    };
  }
}

#endif
