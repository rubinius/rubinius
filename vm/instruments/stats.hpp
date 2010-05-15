#ifndef RBX_INSTRUMENTS_STATS_HPP
#define RBX_INSTRUMENTS_STATS_HPP

#include "util/optimize.hpp"

#include "builtin/tuple.hpp"

#include "instruments/timing.hpp"

namespace rubinius {
  class Integer;
  class LookupTable;
  class VM;
}

namespace stats {
  /* A linear clock. A "normal" clock could be considered a linear clock that
   * reports elapsed time modulo 12 or 24 hours. This clock reports elapsed
   * linear time. The clock continues to report the elapsed time recorded at
   * the point stop() was called.
   */
  class Clock {
    uint64_t start_;
    uint64_t elapsed_;
    bool     started_;

  public:

    Clock()
      : start_(0),
        elapsed_(0),
        started_(false)
    { }

    void clear() {
      start_ = 0;
      started_ = false;
    }

    void start() {
      if(started_) return;

      started_ = true;
      start_ = get_current_time();
    }

    void stop() {
      started_ = false;
      mark();
    }

    void mark() {
      elapsed_ = get_current_time() - start_;
    }

    uint64_t elapsed() {
      if(started_) mark();

      return elapsed_;
    }

    /* Creates a Ruby object representation of the data. */
    rubinius::Integer* to_ruby(rubinius::VM* state);
  };

  /* An accumulating increment timer. Keeps track of the maximum and minimum
   * intervals recorded. Calculates a cumulative moving average according to
   * the formula:
   *
   *               x(i+1) + i*CA(i)
   *    CA(i+1) = -----------------
   *                   i + 1
   *
   *  where:
   *
   *    CA(n) is the nth cumulative average
   *    x(n) is the nth measured value of x
   *    i is the number of measurements
   */
  class Timer {
  protected:

    uint64_t total_;
    uint64_t timings_;
    uint64_t max_;
    uint64_t min_;
    uint64_t last_;
    uint64_t start_;
    double   moving_average_;
    bool     started_;

  public:

    Timer()
      : total_(0)
      , timings_(0)
      , max_(0)
      , min_(0)
      , last_(0)
      , start_(0)
      , moving_average_(0.0)
      , started_(false)
    { }

    bool started() {
      return started_;
    }

    uint64_t total() {
      return total_;
    }

    uint64_t timings() {
      return timings_;
    }

    uint64_t max() {
      return max_;
    }

    uint64_t min() {
      return min_;
    }

    double moving_average() {
      return moving_average_;
    }

    void start() {
      if(started_) return;

      started_ = true;
      start_ = get_current_time();
    }

    void stop() {
      if(!started_) return;

      started_ = false;

      last_ = get_current_time() - start_;
      total_ += last_;

      if(min_ == 0 || min_ > last_) min_ = last_;
      if(max_ == 0 || max_ < last_) max_ = last_;

      moving_average_ = (last_ + timings_ * moving_average_) / (timings_ + 1);
      ++timings_;
    }

    /* Creates a Ruby object representation of the data. */
    rubinius::LookupTable* to_ruby(rubinius::VM* state);
  };

  class StackTimer : public Timer {
    size_t   entered_;
    uint64_t count_;

  public:
    StackTimer()
      : entered_(0)
      , count_(0)
    { }

    uint64_t count() {
      return count_;
    }

    void start() {
      ++entered_;
      Timer::start();
    }

    void stop() {
      if(!started_) return;

      ++count_;
      if(--entered_ == 0) Timer::stop();
    }
  };

  /* A monotonically increasing count. */
  class Counter {
    uint64_t count_;

  public:

    Counter() : count_(0) { }

    uint64_t operator()() {
      return count_;
    }

    uint64_t operator+=(unsigned int num) {
      return count_ += num;
    }

    uint64_t operator++(int) {
      return count_ += 1;
    }

    /* Creates a Ruby object representation of the data. */
    rubinius::Integer* to_ruby(rubinius::VM* state);
  };

  /* Monotonically increasing count where counts are taken in sets. Records
   * the max, min, and moving average of sets of counts, as well as total
   * counts.
   */
  class SetCounter {
    uint64_t total_;
    uint64_t sets_;
    uint64_t set_;
    uint64_t max_;
    uint64_t min_;
    double   moving_average_;
    bool     started_;

  public:

    SetCounter()
      : total_(0),
        sets_(0),
        set_(0),
        max_(0),
        min_(0),
        moving_average_(0.0),
        started_(false)
    { }

    void start() {
      if(started_) return;

      started_ = true;
      set_ = 0;
    }

    void stop() {
      if(!started_) return;

      started_ = false;

      total_ += set_;

      if(min_ == 0 || min_ > set_) min_ = set_;
      if(max_ == 0 || max_ < set_) max_ = set_;

      moving_average_ = (set_ + sets_ * moving_average_) / (sets_ + 1);
      ++sets_;
    }

    uint64_t operator()() {
      return total_;
    }

    /* Allowing this to increment total directly if not recording a set
     * enables gathering stats for all events while focusing the set
     * statistics on only a part of the updates. To force the set stats to be
     * correlated with the total, just ensure this is called after start().
     */
    uint64_t operator++(int) {
      return *this += 1;
    }

    uint64_t operator+=(unsigned int num) {
      if(started_) {
        return set_ += num;
      } else {
        return total_ += num;
      }
    }

    uint64_t max() {
      return max_;
    }

    uint64_t min() {
      return min_;
    }

    double moving_average() {
      return moving_average_;
    }

    /* Creates a Ruby object representation of the data. */
    rubinius::LookupTable* to_ruby(rubinius::VM* state);
  };

  /* A template class for working with collections of counters or timers. The
   * emphasis is on simple indexed access like the following:
   *
   *   stats::GCStats::get()->lifetimes[obj->age]++
   *
   * where the code does not need to know beforehand how old the objects may
   * live. Instances of Counter (in the example above) are added as needed
   * when accessed.
   */
  template<class T>
    class Multi {
      std::vector<T> multi_;

    public:

      T& operator[](int index);

      rubinius::Tuple* to_ruby(rubinius::VM* state);
    };

  /* We could require size_t for index, but that just complicates code at the
   * callsite. Instead, ensure we don't have a negative value and cast.
   */
  template<class T>
    T& Multi<T>::operator[](int index) {
      if(index < 0) index = -index;

      if(static_cast<size_t>(index) >= multi_.size()) {
        multi_.resize(index + 1);
      }

      return multi_[index];
    }

  template<class T>
    rubinius::Tuple* Multi<T>::to_ruby(rubinius::VM* state) {
      rubinius::Tuple* tuple = rubinius::Tuple::create(state, multi_.size());

      for(size_t i = 0; i < multi_.size(); i++) {
        tuple->put(state, i, multi_[i].to_ruby(state));
      }

      return tuple;
    }

  /* Provides various counters and timers for tracking the operation of the
   * generational garbage collector.
   */

  class GCStats {
    static GCStats* stats_;

  public:

    Clock clock;

    // Young generation stats
    Timer allocate_young;
    Timer collect_young;

    Counter young_bytes_allocated;
    SetCounter bytes_copied;
    SetCounter objects_copied;
    SetCounter objects_promoted;

    Multi<Counter> young_object_types;
    Multi<Counter> lifetimes;

    // Mature generation stats
    Timer allocate_mature;
    Timer collect_mature;

    Counter mature_bytes_allocated;
    Counter large_objects;
    Counter chunks_added;
    SetCounter objects_seen;

    Multi<Counter> mature_object_types;

  public:

    GCStats() {
      clock.start();
    }

    static GCStats* get() {
      if(unlikely(!stats_)) stats_ = new GCStats;
      return stats_;
    }

    static void clear() {
      if(stats_) delete stats_;
      stats_ = new GCStats;
    }

    /* Creates a Ruby object representation of the data. */
    rubinius::LookupTable* to_ruby(rubinius::VM* state);
  };
};

#endif
