#include "vm.hpp"
#include "metrics.hpp"

#include "object_utils.hpp"
#include "shared_state.hpp"
#include "configuration.hpp"
#include "ontology.hpp"

#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/integer.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

#include "gc/managed.hpp"

#include "dtrace/dtrace.h"

#include "util/logger.hpp"

#include <fcntl.h>
#include <stdint.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include <ostream>

namespace rubinius {
  using namespace utilities;

  namespace metrics {
    void RubyMetrics::add(MetricsData& data) {
      if(data.type != eRubyMetrics) return;

      add(data.m.ruby_metrics);
    }

    void FinalizerMetrics::add(MetricsData& data) {
      if(data.type != eFinalizerMetrics) return;

      add(data.m.finalizer_metrics);
    }

    void JITMetrics::add(MetricsData& data) {
      if(data.type != eJITMetrics) return;

      add(data.m.jit_metrics);
    }

    void ConsoleMetrics::add(MetricsData& data) {
      if(data.type != eConsoleMetrics) return;

      add(data.m.console_metrics);
    }

    void SystemMetrics::add(MetricsData& data) {
      add(data.system_metrics);
    }

    FileEmitter::FileEmitter(MetricsMap& map, std::string path)
      : MetricsEmitter()
      , metrics_map_(map)
      , path_(path)
      , fd_(-1)
    {
      initialize();
    }

    FileEmitter::~FileEmitter() {
      cleanup();
    }

#define RBX_METRICS_FILE_BUFLEN   256

    void FileEmitter::send_metrics() {
      char buf[RBX_METRICS_FILE_BUFLEN];

      for(MetricsMap::iterator i = metrics_map_.begin();
          i != metrics_map_.end();
          ++i)
      {
        snprintf(buf, RBX_METRICS_FILE_BUFLEN, "%s%lld",
            i == metrics_map_.begin() ? "" : " ", (long long unsigned int)(*i)->second);
        if(write(fd_, buf, strlen(buf)) < 0) {
          logger::error("%s: unable to write file metrics", strerror(errno));
        }
      }

      if(write(fd_, "\n", 1)) {
        logger::error("%s: unable to write file metrics", strerror(errno));
      }
    }

    void FileEmitter::initialize() {
      if(!(fd_ = ::open(path_.c_str(), O_CREAT | O_WRONLY | O_CLOEXEC, 0660))) {
        logger::error("%s: unable to open metrics file", strerror(errno));
      }

      if(lseek(fd_, 0, SEEK_END) == 0) {
        char buf[RBX_METRICS_FILE_BUFLEN];

        for(MetricsMap::iterator i = metrics_map_.begin();
            i != metrics_map_.end();
            ++i)
        {
          snprintf(buf, RBX_METRICS_FILE_BUFLEN, "%s%s",
              i == metrics_map_.begin() ? "" : ", ", (*i)->first.c_str());
          if(write(fd_, buf, strlen(buf)) < 0) {
            logger::error("%s: unable to write file metrics", strerror(errno));
          }
        }

        if(write(fd_, "\n", 1)) {
          logger::error("%s: unable to write file metrics", strerror(errno));
        }
      }
    }

    void FileEmitter::cleanup() {
      if(fd_ > 0) {
        close(fd_);
        fd_ = -1;
      }
    }

    void FileEmitter::reinit() {
      // Don't turn on FileEmitter in children by default.
      cleanup();
    }

    StatsDEmitter::StatsDEmitter(MetricsMap& map, std::string server, std::string prefix)
      : MetricsEmitter()
      , metrics_map_(map)
      , socket_fd_(-1)
    {
      size_t colon = server.find(':');
      if(colon == std::string::npos) {
        host_ = "localhost";
        if(server.size() > 0) {
          port_ = server;
        } else {
          port_ = "8125";
        }
      } else {
        host_ = server.substr(0, colon);
        port_ = server.substr(colon + 1);
      }

      size_t index = prefix.find("$nodename");
      if(index != std::string::npos) {
        struct utsname name;

        if(uname(&name)) {
          prefix_ = "";
        } else {
          std::ostringstream parts;
          std::string n = name.nodename;

          for(size_t p = n.size(), i = p; i != 0; p = i - 1) {
            if((i = n.rfind('.', p)) == std::string::npos) {
              parts << n.substr(0, p + 1);
              break;
            } else {
              parts << n.substr(i + 1, p - i) << ".";
            }

          }

          prefix_ = prefix;
          prefix_.replace(index, strlen("$nodename"), parts.str());
        }
      } else {
        prefix_ = prefix;
      }

      index = prefix_.find("$pid");
      if(index != std::string::npos) {
        std::ostringstream pid;
        pid << getpid();

        prefix_.replace(index, strlen("$pid"), pid.str());
      }

      if(prefix_.size() > 0) prefix_ += ".";

      initialize();
    }

    StatsDEmitter::~StatsDEmitter() {
      cleanup();
    }

#define RBX_METRICS_STATSD_BUFLEN   256

    void StatsDEmitter::send_metrics() {
      char buf[RBX_METRICS_STATSD_BUFLEN];

      for(MetricsMap::iterator i = metrics_map_.begin();
          i != metrics_map_.end();
          ++i)
      {
        snprintf(buf, RBX_METRICS_STATSD_BUFLEN, "%s%s:%lld|g",
            prefix_.c_str(), (*i)->first.c_str(), (long long unsigned int)(*i)->second);
        if(send(socket_fd_, buf, strlen(buf), 0) < 0) {
          logger::error("%s: unable to send StatsD metrics", strerror(errno));
        }
      }
    }

    void StatsDEmitter::initialize() {
      struct addrinfo hints, *res, *res0;

      memset(&hints, 0, sizeof(hints));
      hints.ai_family = PF_UNSPEC;
      hints.ai_socktype = SOCK_DGRAM;

      if(int error = getaddrinfo(host_.c_str(), port_.c_str(), &hints, &res0) < 0) {
        logger::error("%s: unable to get StatsD server address info",
            gai_strerror(error));
        return;
      }

      for(res = res0; res; res = res->ai_next) {
        if((socket_fd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
          continue;
        }

        if(connect(socket_fd_, res->ai_addr, res->ai_addrlen) < 0) {
          close(socket_fd_);
          socket_fd_ = -1;
          continue;
        }

        break;
      }

      freeaddrinfo(res0);

      if(socket_fd_ < 0) {
        logger::error("unable to create StatsD UDP socket");
        return;
      }

      fcntl(socket_fd_, F_SETFL, O_NONBLOCK);
    }

    void StatsDEmitter::cleanup() {
      if(socket_fd_ > 0) close(socket_fd_);
    }

    void StatsDEmitter::reinit() {
      cleanup();
      initialize();
    }

    Metrics::Metrics(STATE)
      : InternalThread(state, "rbx.metrics", InternalThread::eSmall)
      , enabled_(true)
      , values_(state)
      , interval_(state->shared().config.system_metrics_interval)
      , timer_(NULL)
      , emitter_(NULL)
    {
      map_metrics();

      if(!state->shared().config.system_metrics_target.value.compare("statsd")) {
        emitter_ = new StatsDEmitter(metrics_map_,
            state->shared().config.system_metrics_statsd_server.value,
            state->shared().config.system_metrics_statsd_prefix.value);
      } else if(state->shared().config.system_metrics_target.value.compare("none")) {
        emitter_ = new FileEmitter(metrics_map_,
            state->shared().config.system_metrics_target.value);
      }
    }

    Metrics::~Metrics() {
      if(timer_) delete timer_;
      if(emitter_) delete emitter_;

      for(MetricsMap::iterator i = metrics_map_.begin();
          i != metrics_map_.end();
          ++i)
      {
        delete (*i);
      }
    }

    void Metrics::map_metrics() {
      // JIT metrics
      metrics_map_.push_back(new MetricsItem(
            "jit.methods.queued", metrics_collection_.jit_metrics.methods_queued));
      metrics_map_.push_back(new MetricsItem(
            "jit.methods.compiled", metrics_collection_.jit_metrics.methods_compiled));
      metrics_map_.push_back(new MetricsItem(
            "jit.methods.failed", metrics_collection_.jit_metrics.methods_failed));
      metrics_map_.push_back(new MetricsItem(
            "jit.bytes", metrics_collection_.jit_metrics.bytes));
      metrics_map_.push_back(new MetricsItem(
            "jit.time.us", metrics_collection_.jit_metrics.time_us));
      metrics_map_.push_back(new MetricsItem(
            "jit.uncommon_exits", metrics_collection_.jit_metrics.uncommon_exits));
      metrics_map_.push_back(new MetricsItem(
            "jit.inlined.accessors", metrics_collection_.jit_metrics.inlined_accessors));
      metrics_map_.push_back(new MetricsItem(
            "jit.inlined.methods", metrics_collection_.jit_metrics.inlined_methods));
      metrics_map_.push_back(new MetricsItem(
            "jit.inlined.blocks", metrics_collection_.jit_metrics.inlined_blocks));

      // Object memory metrics
      metrics_map_.push_back(new MetricsItem(
            "memory.young.bytes",
            metrics_collection_.ruby_metrics.memory_young_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.young.objects",
            metrics_collection_.ruby_metrics.memory_young_objects));
      metrics_map_.push_back(new MetricsItem(
            "memory.young.occupancy",
            metrics_collection_.ruby_metrics.memory_young_occupancy));
      metrics_map_.push_back(new MetricsItem(
            "memory.immix.bytes",
            metrics_collection_.ruby_metrics.memory_immix_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.immix.objects",
            metrics_collection_.ruby_metrics.memory_immix_objects));
      metrics_map_.push_back(new MetricsItem(
            "memory.immix.chunks",
            metrics_collection_.ruby_metrics.memory_immix_chunks));
      metrics_map_.push_back(new MetricsItem(
            "memory.large.bytes",
            metrics_collection_.ruby_metrics.memory_large_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.large.objects",
            metrics_collection_.ruby_metrics.memory_large_objects));
      metrics_map_.push_back(new MetricsItem(
            "memory.symbols",
            metrics_collection_.ruby_metrics.memory_symbols));
      metrics_map_.push_back(new MetricsItem(
            "memory.symbols.bytes",
            metrics_collection_.ruby_metrics.memory_symbols_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.code.bytes", metrics_collection_.ruby_metrics.memory_code_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.promoted.bytes",
            metrics_collection_.ruby_metrics.memory_promoted_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.promoted.objects",
            metrics_collection_.ruby_metrics.memory_promoted_objects));
      metrics_map_.push_back(new MetricsItem(
            "memory.slab.refills",
            metrics_collection_.ruby_metrics.memory_slab_refills));
      metrics_map_.push_back(new MetricsItem(
            "memory.slab.refills.fails",
            metrics_collection_.ruby_metrics.memory_slab_refills_fails));
      metrics_map_.push_back(new MetricsItem(
            "memory.data_objects",
            metrics_collection_.ruby_metrics.memory_data_objects));
      metrics_map_.push_back(new MetricsItem(
            "memory.capi_handles",
            metrics_collection_.ruby_metrics.memory_capi_handles));
      metrics_map_.push_back(new MetricsItem(
            "memory.inflated_headers",
            metrics_collection_.ruby_metrics.memory_inflated_headers));

      // Garbage collector metrics
      metrics_map_.push_back(new MetricsItem(
            "gc.young.count", metrics_collection_.ruby_metrics.gc_young_count));
      metrics_map_.push_back(new MetricsItem(
            "gc.young.ms", metrics_collection_.ruby_metrics.gc_young_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.immix.count", metrics_collection_.ruby_metrics.gc_immix_count));
      metrics_map_.push_back(new MetricsItem(
            "gc.immix.stop.ms",
            metrics_collection_.ruby_metrics.gc_immix_stop_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.immix.concurrent.ms",
            metrics_collection_.ruby_metrics.gc_immix_concurrent_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.large.count", metrics_collection_.ruby_metrics.gc_large_count));
      metrics_map_.push_back(new MetricsItem(
            "gc.large.sweep.ms",
            metrics_collection_.ruby_metrics.gc_large_sweep_ms));

      // I/O metrics
      metrics_map_.push_back(new MetricsItem(
            "io.read.bytes", metrics_collection_.system_metrics.io_read_bytes));
      metrics_map_.push_back(new MetricsItem(
            "io.write.bytes", metrics_collection_.system_metrics.io_write_bytes));

      // OS activity metrics
      metrics_map_.push_back(new MetricsItem(
            "os.signals.received",
            metrics_collection_.system_metrics.os_signals_received));
      metrics_map_.push_back(new MetricsItem(
            "os.signals.processed",
            metrics_collection_.system_metrics.os_signals_processed));

      // VM metrics
      metrics_map_.push_back(new MetricsItem(
            "vm.inline_cache.resets",
            metrics_collection_.system_metrics.vm_inline_cache_resets));
      metrics_map_.push_back(new MetricsItem(
            "vm.threads.created",
            metrics_collection_.system_metrics.vm_threads_created));
      metrics_map_.push_back(new MetricsItem(
            "vm.threads.destroyed",
            metrics_collection_.system_metrics.vm_threads_destroyed));

      // Lock metrics
      metrics_map_.push_back(new MetricsItem(
            "locks.stop_the_world.ns",
            metrics_collection_.system_metrics.locks_stop_the_world_ns));
    }

    void Metrics::init_ruby_metrics(STATE) {
      LookupTable* map = LookupTable::create(state);
      Module* mod = as<Module>(G(rubinius)->get_const(state, state->symbol("Metrics")));
      mod->set_const(state, "Map", map);

      Tuple* values = Tuple::create(state, metrics_map_.size());
      values_.set(values);
      mod->set_const(state, "Values", values);

      int index = 0;

      for(MetricsMap::iterator i = metrics_map_.begin();
          i != metrics_map_.end();
          ++i)
      {
        values->put(state, index, Integer::from(state, (*i)->second));

        Object* key = reinterpret_cast<Object*>(state->symbol((*i)->first.c_str()));
        map->store(state, key, Fixnum::from(index++));
      }
    }

    void Metrics::update_ruby_values(STATE) {
      GCDependent guard(state, 0);

      Tuple* values = values_.get();
      int index = 0;

      for(MetricsMap::iterator i = metrics_map_.begin();
          i != metrics_map_.end();
          ++i)
      {
        values->put(state, index++, Integer::from(state, (*i)->second));
      }
    }

    void Metrics::initialize(STATE) {
      InternalThread::initialize(state);

      enabled_ = true;

      timer_ = new timer::Timer;

      metrics_lock_.init();
      metrics_collection_.init();
      metrics_history_.init();
    }

    void Metrics::wakeup(STATE) {
      InternalThread::wakeup(state);

      if(timer_) {
        timer_->clear();
        timer_->cancel();
      }
    }

    void Metrics::after_fork_child(STATE) {
      if(emitter_) emitter_->reinit();

      InternalThread::after_fork_child(state);
    }

    void Metrics::add_historical_metrics(MetricsData& metrics) {
      if(!enabled_) return;

      {
        utilities::thread::Mutex::LockGuard guard(metrics_lock_);

        metrics_history_.add(metrics);
      }
    }

    void Metrics::run(STATE) {
      timer_->set(interval_);

      while(!thread_exit_) {
        if(timer_->wait_for_tick() < 0) {
          logger::error("metrics: error waiting for timer, exiting thread");
          break;
        }

        if(thread_exit_) break;

        metrics_collection_.init();
        ThreadList* threads = state->shared().threads();

        for(ThreadList::iterator i = threads->begin();
            i != threads->end();
            ++i) {
          if(VM* vm = (*i)->as_vm()) {
            metrics_collection_.add(vm->metrics());
          }
        }

#ifdef ENABLE_LLVM
        if(LLVMState* llvm_state = state->shared().llvm_state) {
          metrics_collection_.add(llvm_state->metrics());
        }
#endif

        {
          utilities::thread::Mutex::LockGuard guard(metrics_lock_);

          metrics_collection_.add(metrics_history_);
        }

        update_ruby_values(state);

        if(emitter_) emitter_->send_metrics();
      }

      timer_->clear();
    }
  }
}
