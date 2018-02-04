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
        std::string location;
        std::string module_name;
        uint64_t samples;
        uint64_t invokes;

        IndexEntry(std::string& l, std::string& m, uint64_t s, uint64_t i)
          : location(l)
          , module_name(m)
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

        Entry(uint64_t s, int h, std::string& r)
          : serial(s)
          , cache_hits(h)
          , receiver_class(r)
        {
        }
      };

      std::unordered_map<uint64_t, IndexEntry> index_;
      std::unordered_map<ProfilerEntryKey, Entry> entries_;

      bool collecting_;
      bool reporting_;
      bool updating_;

    public:
      Profiler()
        : Diagnostic()
        , index_()
        , entries_()
        , collecting_(false)
        , reporting_(false)
        , updating_(false)
      {
        set_type("Profiler");
      }

      virtual ~Profiler() { }

      virtual void start_reporting(STATE) {
        if(state->shared().config.diagnostics_profiler_enabled) {
          Diagnostic::start_reporting(state);
          reporting_ = true;
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->shared().config.diagnostics_profiler_enabled) {
          Diagnostic::stop_reporting(state);
          reporting_ = false;
        }
      }

      virtual void update() {
        updating_ = true;
      }

      void start_collecting() {
        if(!updating_) return;

        entries_.clear();
        collecting_ = true;
      }

      void stop_collecting() {
        collecting_ = false;
      }

      bool collecting_p() const {
        return collecting_;
      }

      virtual void add_index() {
      }

      virtual void add_entry(uint64_t caller_serial, int ip,
          uint64_t callee_serial, int hits, std::string& receiver_class)
      {
        entries_.insert(std::make_pair(ProfilerEntryKey(caller_serial, ip),
              Entry(callee_serial, hits, receiver_class)));
      }
    };
  }
}

#endif
