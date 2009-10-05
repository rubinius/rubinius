#include "instruments/stats.hpp"

#include "vm.hpp"

#include "builtin/float.hpp"
#include "builtin/integer.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/symbol.hpp"

using namespace rubinius;

namespace stats {
  GCStats* GCStats::stats_ = 0;

  /* These convenience methods may not be necessary, but
   * make the code a bit cleaner.
   */
  static Symbol* total(VM* state) {
    return state->symbol("total");
  }

  static Symbol* timings(VM* state) {
    return state->symbol("timings");
  }

  static Symbol* max(VM* state) {
    return state->symbol("max");
  }

  static Symbol* min(VM* state) {
    return state->symbol("min");
  }

  static Symbol* average(VM* state) {
    return state->symbol("average");
  }

  static Symbol* bytes_allocated(VM* state) {
    return state->symbol("bytes_allocated");
  }

  Integer* Clock::to_ruby(VM* state) {
    return Integer::from(state, elapsed());
  }

  LookupTable* Timer::to_ruby(VM* state) {
    LookupTable* tbl = LookupTable::create(state);

    tbl->store(state, stats::total(state), Integer::from(state, total()));
    tbl->store(state, stats::timings(state), Integer::from(state, timings()));
    tbl->store(state, stats::max(state), Integer::from(state, max()));
    tbl->store(state, stats::min(state), Integer::from(state, min()));
    tbl->store(state, stats::average(state), Float::create(state, moving_average()));

    return tbl;
  }

  Integer* Counter::to_ruby(VM* state) {
    return Integer::from(state, count_);
  }

  LookupTable* SetCounter::to_ruby(VM* state) {
    LookupTable* tbl = LookupTable::create(state);

    tbl->store(state, stats::total(state), Integer::from(state, total_));
    tbl->store(state, stats::max(state), Integer::from(state, max()));
    tbl->store(state, stats::min(state), Integer::from(state, min()));
    tbl->store(state, stats::average(state), Float::create(state, moving_average()));

    return tbl;
  }

  LookupTable* GCStats::to_ruby(VM* state) {
    LookupTable* tbl;
    LookupTable* gc_tbl = LookupTable::create(state);

    tbl = collect_young.to_ruby(state);
    tbl->store(state, state->symbol("objects_copied"), objects_copied.to_ruby(state));
    tbl->store(state, state->symbol("bytes_copied"), bytes_copied.to_ruby(state));
    tbl->store(state, state->symbol("objects_promoted"), objects_promoted.to_ruby(state));
    tbl->store(state, state->symbol("lifetimes"), lifetimes.to_ruby(state));
    gc_tbl->store(state, state->symbol("collect_young"), tbl);

    tbl = allocate_young.to_ruby(state);
    tbl->store(state, bytes_allocated(state), young_bytes_allocated.to_ruby(state));
    tbl->store(state, state->symbol("object_types"), young_object_types.to_ruby(state));
    gc_tbl->store(state, state->symbol("allocate_young"), tbl);

    tbl = collect_mature.to_ruby(state);
    tbl->store(state, state->symbol("objects_seen"), objects_seen.to_ruby(state));
    gc_tbl->store(state, state->symbol("collect_mature"), tbl);

    tbl = allocate_mature.to_ruby(state);
    tbl->store(state, bytes_allocated(state), mature_bytes_allocated.to_ruby(state));
    tbl->store(state, state->symbol("chunks_added"), chunks_added.to_ruby(state));
    tbl->store(state, state->symbol("large_objects"), large_objects.to_ruby(state));
    tbl->store(state, state->symbol("object_types"), mature_object_types.to_ruby(state));
    gc_tbl->store(state, state->symbol("allocate_mature"), tbl);

    gc_tbl->store(state, state->symbol("clock"), clock.to_ruby(state));

    return gc_tbl;
  }
};
