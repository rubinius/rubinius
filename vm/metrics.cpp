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
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include <ostream>

namespace rubinius {
  using namespace utilities;

  namespace metrics {
    void RubyMetrics::add(MetricsData* data) {
      if(data->type != eRubyMetrics) return;

      add(&data->m.ruby_metrics);
    }

    void FinalizerMetrics::add(MetricsData* data) {
      if(data->type != eFinalizerMetrics) return;

      add(&data->m.finalizer_metrics);
    }

    void JITMetrics::add(MetricsData* data) {
      if(data->type != eJITMetrics) return;

      add(&data->m.jit_metrics);
    }

    void ConsoleMetrics::add(MetricsData* data) {
      if(data->type != eConsoleMetrics) return;

      add(&data->m.console_metrics);
    }

    void SystemMetrics::add(MetricsData* data) {
      add(&data->system_metrics);
    }

    Object* metrics_trampoline(STATE) {
      state->shared().metrics()->process_metrics(state);
      GCTokenImpl gct;
      state->gc_dependent(gct, 0);
      return cNil;
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
              parts << n.substr(0, p);
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
      : AuxiliaryThread()
      , shared_(state->shared())
      , vm_(NULL)
      , enabled_(true)
      , thread_exit_(false)
      , thread_running_(false)
      , thread_(state)
      , values_(state)
      , interval_(state->shared().config.system_metrics_interval)
      , timer_(NULL)
      , emitter_(NULL)
    {
      metrics_lock_.init();
      map_metrics();

      if(!shared_.config.system_metrics_target.value.compare("statsd")) {
        emitter_ = new StatsDEmitter(metrics_map_,
            shared_.config.system_metrics_statsd_server.value,
            shared_.config.system_metrics_statsd_prefix.value);
      }

      shared_.auxiliary_threads()->register_thread(this);
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

      shared_.auxiliary_threads()->unregister_thread(this);
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
            "jit.time.last.us", metrics_collection_.jit_metrics.time_last_us));
      metrics_map_.push_back(new MetricsItem(
            "jit.time.total.us", metrics_collection_.jit_metrics.time_total_us));

      // Object memory metrics
      metrics_map_.push_back(new MetricsItem(
            "memory.young.bytes.current",
            metrics_collection_.ruby_metrics.memory_young_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.young.bytes.total",
            metrics_collection_.ruby_metrics.memory_young_bytes_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.young.objects.current",
            metrics_collection_.ruby_metrics.memory_young_objects));
      metrics_map_.push_back(new MetricsItem(
            "memory.young.objects.total",
            metrics_collection_.ruby_metrics.memory_young_objects_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.young.percent_used",
            metrics_collection_.ruby_metrics.memory_young_percent_used));
      metrics_map_.push_back(new MetricsItem(
            "memory.immix.bytes.current",
            metrics_collection_.ruby_metrics.memory_immix_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.immix.bytes.total",
            metrics_collection_.ruby_metrics.memory_immix_bytes_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.immix.objects.current",
            metrics_collection_.ruby_metrics.memory_immix_objects));
      metrics_map_.push_back(new MetricsItem(
            "memory.immix.objects.total",
            metrics_collection_.ruby_metrics.memory_immix_objects_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.immix.chunks.current",
            metrics_collection_.ruby_metrics.memory_immix_chunks));
      metrics_map_.push_back(new MetricsItem(
            "memory.immix.chunks.total",
            metrics_collection_.ruby_metrics.memory_immix_chunks_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.large.bytes.current",
            metrics_collection_.ruby_metrics.memory_large_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.large.bytes.total",
            metrics_collection_.ruby_metrics.memory_large_bytes_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.large.objects.current",
            metrics_collection_.ruby_metrics.memory_large_objects));
      metrics_map_.push_back(new MetricsItem(
            "memory.large.objects.total",
            metrics_collection_.ruby_metrics.memory_large_objects_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.symbols.bytes",
            metrics_collection_.ruby_metrics.memory_symbols_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.code.bytes", metrics_collection_.ruby_metrics.memory_code_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.jit.bytes", metrics_collection_.ruby_metrics.memory_jit_bytes));
      metrics_map_.push_back(new MetricsItem(
            "memory.promoted.bytes.total",
            metrics_collection_.ruby_metrics.memory_promoted_bytes_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.promoted.objects.total",
            metrics_collection_.ruby_metrics.memory_promoted_objects_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.slab.refills.total",
            metrics_collection_.ruby_metrics.memory_slab_refills_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.slab.refills.fails",
            metrics_collection_.ruby_metrics.memory_slab_refills_fails));
      metrics_map_.push_back(new MetricsItem(
            "memory.data_objects.total",
            metrics_collection_.ruby_metrics.memory_data_objects_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.capi_handles.total",
            metrics_collection_.ruby_metrics.memory_capi_handles_total));
      metrics_map_.push_back(new MetricsItem(
            "memory.capi_handles.current",
            metrics_collection_.ruby_metrics.memory_capi_handles));
      metrics_map_.push_back(new MetricsItem(
            "memory.inflated_headers",
            metrics_collection_.ruby_metrics.memory_inflated_headers));

      // Garbage collector metrics
      metrics_map_.push_back(new MetricsItem(
            "gc.young.count", metrics_collection_.ruby_metrics.gc_young_count));
      metrics_map_.push_back(new MetricsItem(
            "gc.young.last.ms", metrics_collection_.ruby_metrics.gc_young_last_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.young.total.ms", metrics_collection_.ruby_metrics.gc_young_total_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.young.lifetime", metrics_collection_.ruby_metrics.gc_young_lifetime));
      metrics_map_.push_back(new MetricsItem(
            "gc.immix.count", metrics_collection_.ruby_metrics.gc_immix_count));
      metrics_map_.push_back(new MetricsItem(
            "gc.immix.stop.last.ms",
            metrics_collection_.ruby_metrics.gc_immix_stop_last_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.immix.stop.total.ms",
            metrics_collection_.ruby_metrics.gc_immix_stop_total_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.immix.concurrent.last.ms",
            metrics_collection_.ruby_metrics.gc_immix_conc_last_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.immix.concurrent.total.ms",
            metrics_collection_.ruby_metrics.gc_immix_conc_total_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.large.count", metrics_collection_.ruby_metrics.gc_large_count));
      metrics_map_.push_back(new MetricsItem(
            "gc.large.sweep.last.ms",
            metrics_collection_.ruby_metrics.gc_large_sweep_last_ms));
      metrics_map_.push_back(new MetricsItem(
            "gc.large.sweep.total.ms",
            metrics_collection_.ruby_metrics.gc_large_sweep_total_ms));

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
            "vm.threads.current",
            metrics_collection_.system_metrics.vm_threads));
      metrics_map_.push_back(new MetricsItem(
            "vm.threads.total",
            metrics_collection_.system_metrics.vm_threads_total));

      // Lock metrics
      metrics_map_.push_back(new MetricsItem(
            "locks.stop_the_world.last.ns",
            metrics_collection_.system_metrics.locks_stop_the_world_last_ns));
      metrics_map_.push_back(new MetricsItem(
            "locks.stop_the_world.total.ns",
            metrics_collection_.system_metrics.locks_stop_the_world_total_ns));
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
        values->put(state, index, Bignum::from(state, (*i)->second));

        Object* key = reinterpret_cast<Object*>(state->symbol((*i)->first.c_str()));
        map->store(state, key, Fixnum::from(index++));
      }
    }

    void Metrics::update_ruby_values(STATE) {
      Tuple* values = values_.get();
      int index = 0;

      for(MetricsMap::iterator i = metrics_map_.begin();
          i != metrics_map_.end();
          ++i)
      {
        values->put(state, index++, Integer::from(state, (*i)->second));
      }
    }

    void Metrics::start(STATE) {
      vm_ = NULL;
      enabled_ = true;
      thread_exit_ = false;
      thread_running_ = false;

      timer_ = new timer::Timer;

      metrics_collection_.init();
      metrics_history_.init();

      start_thread(state);
    }

    void Metrics::wakeup() {
      thread_exit_ = true;

      atomic::memory_barrier();

      if(timer_) {
        timer_->clear();
        timer_->cancel();
      }
    }

    void Metrics::start_thread(STATE) {
      SYNC(state);

      if(!vm_) {
        vm_ = state->shared().new_vm();
        thread_exit_ = false;
        thread_.set(Thread::create(state, vm_, G(thread), metrics_trampoline, true));
      }

      if(thread_.get()->fork_attached(state)) {
        rubinius::bug("Unable to start metrics thread");
      }
    }

    void Metrics::stop_thread(STATE) {
      SYNC(state);

      if(vm_) {
        wakeup();

        if(atomic::poll(thread_running_, false)) {
          void* return_value;
          pthread_t os = vm_->os_thread();
          pthread_join(os, &return_value);
        }

        vm_ = NULL;
      }
    }

    void Metrics::shutdown(STATE) {
      stop_thread(state);
    }

    void Metrics::after_fork_child(STATE) {
      metrics_lock_.init();
      vm_ = NULL;

      start(state);
      if(emitter_) emitter_->reinit();
    }

    void Metrics::add_historical_metrics(MetricsData* metrics) {
      if(!enabled_) return;

      {
        utilities::thread::Mutex::LockGuard guard(metrics_lock_);

        metrics_history_.add(metrics);
      }
    }

    void Metrics::process_metrics(STATE) {
      GCTokenImpl gct;
      RBX_DTRACE_CHAR_P thread_name =
        const_cast<RBX_DTRACE_CHAR_P>("rbx.metrics");
      vm_->set_name(thread_name);

      RUBINIUS_THREAD_START(const_cast<RBX_DTRACE_CHAR_P>(thread_name),
                            state->vm()->thread_id(), 1);

      thread_running_ = true;

      state->vm()->thread->hard_unlock(state, gct, 0);
      state->gc_dependent(gct, 0);

      timer_->set(interval_);

      while(!thread_exit_) {
        {
          GCIndependent guard(state, 0);

          if(timer_->wait_for_tick() < 0) {
            logger::error("metrics: error waiting for timer, exiting thread");
            break;
          }
        }

        if(thread_exit_) break;

        {
          utilities::thread::Mutex::LockGuard guard(metrics_lock_);

          metrics_collection_.init();
          ThreadList* threads = state->shared().threads();

          for(ThreadList::iterator i = threads->begin();
              i != threads->end();
              ++i) {
            if(VM* vm = (*i)->as_vm()) {
              if(MetricsData* data = vm->metrics()) {
                metrics_collection_.add(data);
              }
            }
          }

#ifdef ENABLE_LLVM
          if(state->shared().llvm_state) {
            if(Thread* thread = state->shared().llvm_state->thread()) {
              if(VM* vm = thread->vm()) {
                metrics_collection_.add(vm->metrics());
              }
            }
          }
#endif

          metrics_collection_.add(&metrics_history_);

          update_ruby_values(state);
        }

        {
          GCIndependent guard(state, 0);

          if(emitter_) emitter_->send_metrics();
        }
      }

      timer_->clear();

      thread_running_ = false;

      RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CHAR_P>(thread_name),
                           state->vm()->thread_id(), 1);
    }
  }
}
