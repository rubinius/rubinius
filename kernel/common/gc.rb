module GC
  def self.count
    data = stat
    data[:"gc.young.count"] + data[:"gc.immix.count"]
  end

  def self.time
    data = stat
    data[:"gc.young.total.ms"] +
      data[:"gc.immix.stop.total.ms"] +
      data[:"gc.large.sweep.total.ms"]
  end

  def self.stat
    Rubinius::Metrics.data.to_hash
  end

  module Profiler
    @enabled = true
    @since   = 0

    def self.clear
      @since = GC.time
      nil
    end

    def self.disable
      # Treat this like a request that we don't honor.
      ret = !@enabled
      @enabled = false
      ret
    end

    def self.enable
      # We don't support disable, so sure! enabled!
      ret = !@enabled
      @enabled = true
      ret
    end

    def self.enabled?
      @enabled
    end

    def self.report(out = $stdout)
      out.write result
    end

    def self.result
      stats = GC.stat

      out = <<-OUT
Complete process runtime statistics
===================================

Collections
                         Count       Total time / concurrent (ms)   Last time / concurrent (ms)
Young   #{sprintf("% 22d", stats[:'gc.young.count'])} #{sprintf("% 16d             ", stats[:'gc.young.total.ms'])} #{sprintf("% 16d            ", stats[:'gc.young.last.ms'])}
Full    #{sprintf("% 22d", stats[:'gc.immix.count'])} #{sprintf("% 16d / % 10d", stats[:'gc.immix.stop.total.ms'], stats[:'gc.immix.concurrent.total.ms'])} #{sprintf("% 16d / % 10d", stats[:'gc.immix.stop.last.ms'], stats[:'gc.immix.concurrent.last.ms'])}

Allocation
             Objects allocated        Bytes allocated
Young   #{sprintf("% 22d", stats[:'memory.young.objects.total'])} #{sprintf("% 22d", stats[:'memory.young.bytes.total'])}
Promoted#{sprintf("% 22d", stats[:'memory.promoted.objects.total'])} #{sprintf("% 22d", stats[:'memory.promoted.bytes.total'])}
Mature  #{sprintf("% 22d", stats[:'memory.immix.objects.total'])} #{sprintf("% 22d", stats[:'memory.immix.bytes.total'])}


Usage
                    Bytes used
Young   #{sprintf("% 22d", stats[:'memory.young.bytes.current'])}
Mature  #{sprintf("% 22d", stats[:'memory.immix.bytes.current'])}
Large   #{sprintf("% 22d", stats[:'memory.large.bytes.current'])}
Code    #{sprintf("% 22d", stats[:'memory.code.bytes'])}
Symbols #{sprintf("% 22d", stats[:'memory.symbols.bytes'])}
      OUT
    end

    def self.total_time
      (GC.time - @since) / 1000.0
    end
  end
end
