#include "memory/gc.hpp"
#include "memory/mark_sweep.hpp"
#include "memory.hpp"

#include "object_utils.hpp"

#include "class/tuple.hpp"
#include "class/string.hpp"
#include "class/class.hpp"
#include "class/symbol.hpp"

#include "configuration.hpp"
#include "metrics.hpp"

#include "instruments/timing.hpp"

#include "logger.hpp"

#include <iostream>
#include <algorithm>

namespace rubinius {
namespace memory {

  MarkSweepGC::MarkSweepGC(Memory *om, Configuration& config)
    : GarbageCollector(om)
    , diagnostics_(new Diagnostics())
    , collection_threshold(config.gc_marksweep_threshold)
    , next_collection_bytes(collection_threshold)
  {}

  MarkSweepGC::~MarkSweepGC() {
    if(diagnostics()) delete diagnostics();
  }

  void MarkSweepGC::Diagnostics::update() {
  }

  void MarkSweepGC::free_objects() {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end(); ++i) {
      free_object(*i, true);
    }
  }

  Object* MarkSweepGC::allocate(size_t bytes, bool& collect_now) {
    void* mem = malloc(bytes);
    if(!mem) return NULL;

    Object* obj = reinterpret_cast<Object*>(mem);

    diagnostics()->objects_++;
    diagnostics()->bytes_ += bytes;

    memory_->vm()->metrics().memory.large_objects++;
    memory_->vm()->metrics().memory.large_bytes += bytes;

    entries.push_back(obj);

    next_collection_bytes -= bytes;
    if(next_collection_bytes < 0) {
      memory_->vm()->metrics().gc.large_set++;
      collect_now = true;
      next_collection_bytes = collection_threshold;
    }

    obj->init_header(MatureObjectZone, InvalidType);
    obj->set_in_large();

    return obj;
  }

  void MarkSweepGC::free_object(Object* obj, bool fast) {
    diagnostics()->objects_--;
    diagnostics()->bytes_ -= obj->size_in_bytes(memory_->vm());

    if(!fast) {
      delete_object(obj);
    }

    obj->set_zone(UnspecifiedZone);

    free(reinterpret_cast<void*>(obj));
  }

  Object* MarkSweepGC::move_object(Object* orig, size_t bytes,
                                   bool& collect_now)
  {
    Object* obj = allocate(bytes, collect_now);
    memcpy(obj, orig, bytes);

    obj->set_zone(MatureObjectZone);
    obj->set_age(0);

    orig->set_forward(obj);

    return obj;
  }

  Object* MarkSweepGC::copy_object(Object* orig, bool& collect_now) {
    Object* obj = allocate(orig->size_in_bytes(memory_->vm()), collect_now);

    obj->initialize_full_state(memory_->vm(), orig, 0);

    return obj;
  }

  Object* MarkSweepGC::saw_object(Object* obj) {
    if(obj->marked_p(memory_->mark())) return NULL;
    obj->mark(memory_, memory_->mark());

    // Add the object to the mark stack, to be scanned later.
    mark_stack_.push_back(obj);
    return NULL;
  }

  bool MarkSweepGC::mature_gc_in_progress() {
    return memory_->mature_gc_in_progress();
  }

  void MarkSweepGC::after_marked() {
    metrics::MetricsData& metrics = memory_->vm()->metrics();

    timer::StopWatch<timer::microseconds> timer(metrics.gc.large_sweep_us);

    // Cleanup all weakrefs seen
    clean_weakrefs();

    // Sweep up the garbage
    sweep_objects();
  }

  void MarkSweepGC::sweep_objects() {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end();) {
      Object* obj = *i;
      if(obj->marked_p(memory_->mark())) {
        ++i;
      } else {
        free_object(obj);
        i = entries.erase(i);
      }
    }
  }

  ObjectPosition MarkSweepGC::validate_object(Object* obj) {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end(); ++i) {
      if(*i == obj) return cMatureObject;
    }

    return cUnknown;
  }
}
}
