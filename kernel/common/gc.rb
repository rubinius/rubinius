module GC
  class CompositeKeyHash
    def initialize(data=nil, prefix=nil)
      @data = data || {}
      @keys = data ? {} : @data
      @prefix = prefix
    end

    def key?(key)
      @keys.key? key or (!@prefix and @data.keys.any? { |k| k =~ /^#{key}(\.|$)?/ })
    end

    def [](key)
      return @keys[key] if @keys.key? key

      prefix = [@prefix, key].compact.join(".").to_sym
      return @data[prefix] if @data.key? prefix

      if @data.keys.any? { |k| k =~ /^#{prefix}(\.|$)/ }
        return @keys[key] = self.class.new(@data, prefix)
      end
    end

    def []=(key, value)
      @keys[key] = value
    end

    def method_missing(m, *args)
      @data.send m, *args
    end
  end

  def self.count
    Rubinius::GC.count
  end

  def self.stat
    @stats = Rubinius::GC.stat @stats
    @metrics ||= CompositeKeyHash.new

    i = 0
    total = @stats.size

    while i < total
      @metrics[@stats[i]] = @stats[i + 1]
      i += 2
    end

    @metrics[:count] = @metrics[:'gc.young.count'] + @metrics[:'gc.full.count']

    @metrics
  end

  module Profiler
    @enabled = true
    @since   = 0

    def self.clear
      @since = Rubinius::GC.time
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
Young   #{sprintf("% 22d", stats[:'gc.young.count'])} #{sprintf("% 16d             ", stats[:'gc.young.total_wallclock'])} #{sprintf("% 16d            ", stats[:'gc.young.last_wallclock'])}
Full    #{sprintf("% 22d", stats[:'gc.full.count'])} #{sprintf("% 16d / % 10d", stats[:'gc.full.total_stop_wallclock'], stats[:'gc.full.total_concurrent_wallclock'])} #{sprintf("% 16d / % 10d", stats[:'gc.full.last_stop_wallclock'], stats[:'gc.full.last_concurrent_wallclock'])}

Allocation
             Objects allocated        Bytes allocated
Young   #{sprintf("% 22d", stats[:'memory.counter.young_objects'])} #{sprintf("% 22d", stats[:'memory.counter.young_bytes'])}
Promoted#{sprintf("% 22d", stats[:'memory.counter.promoted_objects'])} #{sprintf("% 22d", stats[:'memory.counter.promoted_bytes'])}
Mature  #{sprintf("% 22d", stats[:'memory.counter.mature_objects'])} #{sprintf("% 22d", stats[:'memory.counter.mature_bytes'])}


Usage
                    Bytes used
Young   #{sprintf("% 22d", stats[:'memory.young.bytes'])}
Mature  #{sprintf("% 22d", stats[:'memory.mature.bytes'])}
Large   #{sprintf("% 22d", stats[:'memory.large.bytes'])}
Code    #{sprintf("% 22d", stats[:'memory.code.bytes'])}
Symbols #{sprintf("% 22d", stats[:'memory.symbols.bytes'])}
      OUT
    end

    def self.total_time
      (Rubinius::GC.time - @since) / 1000.0
    end
  end
end
