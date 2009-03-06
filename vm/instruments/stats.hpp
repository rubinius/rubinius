#ifndef RBX_INSTRUMENTS_STATS_HPP
#define RBX_INSTRUMENTS_STATS_HPP

#include "optimize.hpp"
#include "instruments/timing.hpp"

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
      : total_(0),
        timings_(0),
        max_(0),
        min_(0),
        last_(0),
        start_(0),
        moving_average_(0.0),
        started_(false)
    { }

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

      moving_average_ = (last_ + timings_ * moving_average_) / ++timings_;
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
  };

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

    Counter objects_copied;
    Counter bytes_copied;
    Counter young_bytes_allocated;

    // Mature generation stats
    Timer allocate_mature;
    Timer collect_mature;

    Counter mature_bytes_allocated;

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
  };
};

#endif
