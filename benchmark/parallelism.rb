class ParallelismCalculator
  def initialize(loop_count=100000)
    @loop_count = loop_count
    @inc = 1
    @jit_warned = false
  end

  # It's critical this method take take the same ammount of time
  # relative to +lc+ no matter when it's called. Because of this
  # reason, this script must be run with any form of JIT off, since
  # it will change run's work and throw off the rest of the
  # calculations.
  #
  def run(lc)
    i = 0

    start = Time.now
    while i < lc
      i += @inc
    end

    return Time.now - start
  end

  def calibrate
    lc = @loop_count

    count = 0

    target = Time.now + 1
    start = cur = Time.now
    while cur < target
      run lc

      count += lc
      cur = Time.now
    end

    diff = cur - start

    @count_per_1s = (count.to_f / diff).to_i
  end

  def run_parallel(count=2, work=1)
    calibrate

    wait = true
    threads = count.times.map do
      Thread.new do
        Thread.pass while wait
        run(@count_per_1s * work)
      end
    end

    sleep 1

    before = Time.now
    wait = false

    threads.each { |t| t.join }

    diff = Time.now - before

    if diff < work and !@jit_warned
      warn "WARNING: Highly suspect results (is JIT on?). actual #{diff} < ideal #{work}"
      @jit_warned = true
    end

    Report.new count, work, Time.now - before
  end

  def parallel_median(avg_count=5, count=2, work=1)
    reports = avg_count.times.map {
      run_parallel(count, work)
    }

    Report.median(*reports)
  end

  def parallel_best(avg_count=5, count=2, work=1)
    reports = avg_count.times.map {
      run_parallel(count, work)
    }

    reports.sort_by { |r| r.time }.first
  end

  class Report
    def initialize(count, work, time)
      @count = count
      @work = work
      @time = time
    end

    attr_reader :count, :work, :time

    def ideal
      @work
    end

    def worst
      @work * @count
    end

    def score
      # On the continium from perfect concurrency to zero concurrency,
      # report the position as a higher number of better concurrency.
      #
      # Think of this like an American baseball batting average.

      margin = @time - ideal
      range = worst - ideal

      position = margin / range

      (100 - (100 * position)).to_i
    end

    def describe
      puts "Work units: #{@work * @count}\n"
      puts "Parallel count: #{@count}\n"
      puts "Ideal time: #{ideal}s\n"
      puts "Total time: #{@time}s\n"

      puts "Concurrency score: #{score}"
      puts "Assessment: #{assess}"
    end

    def assess
      case
      when score > 93
        "Fully parallel with little to no overhead"
      when score > 80
        "Fully parallel with minor locking overhead"
      when score < 10
        "No computation parallelism (likely has a GIL)"
      else
        "Somewhat parallel, likely some system locking overhead"
      end
    end

    def self.mean(*reports)
      times = reports.map { |x| x.time }
      sum = times.inject { |a,i| a + i }
      mean = sum / reports.size

      r = reports.first

      Report.new r.count, r.work, mean
    end

    def self.median(*reports)
      sorted = reports.sort_by { |r| r.time }

      mid = reports.size / 2

      if reports.size % 2 == 0
        mean sorted[mid-1], sorted[mid]
      else
        sorted[mid]
      end
    end
  end

end

wu = ParallelismCalculator.new

wu.parallel_best(5, 4, 2).describe
