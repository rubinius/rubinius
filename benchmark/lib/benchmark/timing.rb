if !defined?(RUBY_ENGINE) or RUBY_ENGINE != "rbx"
  require 'rubygems'
end

require 'ffi'

module Benchmark
  module Timing
    extend FFI::Library

    ffi_lib FFI::Library::LIBC

    attach_function :gettimeofday, [:pointer, :pointer], :int

    class TimeVal < FFI::Struct
      layout :tv_sec, :time_t, :tv_usec, :suseconds_t

      def sec
        self[:tv_sec]
      end

      def usec
        self[:tv_usec]
      end

      def update!
        Timing.gettimeofday to_ptr, nil
      end

      def to_time
        Time.at sec, usec
      end

      def <(tv)
        return true if sec < tv.sec
        return true if usec < tv.usec
        return false
      end

      def ==(tv)
        if sec == tv.sec and usec == tv.usec
          return true
        end
        return false
      end

      def diff(tv)
        if tv.sec == sec
          return tv.usec - usec
        end

        sec_diff = tv.sec - sec

        usec_diff = tv.usec - usec
        if usec_diff < 0
          sec_diff -= 1
          usec_diff += 1_000_000
        end

        (sec_diff * 1_000_000) + usec_diff
      end

      def >(tv)
        tv < self
      end

      def elapsed?(tv, second)
        target = sec + second
        cur_tv = tv.sec

        return true if target < cur_tv
        return true if target == cur_tv and usec < tv.usec

        return false
      end

    end

    def self.resolution
      samples = []

      t1 = TimeVal.new
      t2 = TimeVal.new

      30.times do
        t1.update!

        while true
          t2.update!
          break if t2 != t1
        end

        samples << t1.diff(t2)
      end

      sum = samples.inject(0) { |acc, i| acc + i }
      sum / 30
    end

    def self.mean(samples)
      sum = samples.inject(0) { |acc, i| acc + i }
      sum / samples.size
    end

    def self.variance(samples, m=nil)
      m ||= mean(samples)

      total = samples.inject(0) { |acc, i| acc + ((i - m) ** 2) }

      total / samples.size
    end

    def self.stddev(samples, m=nil)
      Math.sqrt variance(samples, m)
    end

    def self.resample_mean(samples, resample_times=100)
      resamples = []

      resample_times.times do
        resample = samples.map { samples[rand(samples.size)] }
        resamples << Timing.mean(resample)
      end

      resamples
    end

    def self.clean_env
      # rbx
      if GC.respond_to? :run
        GC.run(true)
      else
        GC.start
      end
    end
  end
end
