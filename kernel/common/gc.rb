module GC

  def self.count
    Rubinius::GC.count
  end

  def self.stat
    @agent ||= begin
      require 'rubinius/agent'
      Rubinius::Agent.loopback
    end

    stats = {
      :gc => {
        :young => {
          :count => @agent.get('system.gc.young.count')[1],
          :total_wallclock => @agent.get('system.gc.young.total_wallclock')[1],
          :last_wallclock => @agent.get('system.gc.young.last_wallclock')[1]
        },
        :full => {
          :count => @agent.get('system.gc.full.count')[1],
          :total_stop_wallclock => @agent.get('system.gc.full.total_stop_wallclock')[1],
          :total_concurrent_wallclock => @agent.get('system.gc.full.total_concurrent_wallclock')[1],
          :last_stop_wallclock => @agent.get('system.gc.full.last_stop_wallclock')[1],
          :last_concurrent_wallclock => @agent.get('system.gc.full.last_concurrent_wallclock')[1]
        }
      },
      :memory => {
        :counter => {
          :young_objects => @agent.get("system.memory.counter.young_objects")[1],
          :young_bytes => @agent.get("system.memory.counter.young_bytes")[1],
          :promoted_objects => @agent.get("system.memory.counter.promoted_objects")[1],
          :promoted_bytes => @agent.get("system.memory.counter.promoted_bytes")[1],
          :mature_objects => @agent.get("system.memory.counter.mature_objects")[1],
          :mature_bytes => @agent.get("system.memory.counter.mature_bytes")[1],
        },
        :young => {:bytes => @agent.get("system.memory.young.bytes")[1]},
        :mature => {:bytes => @agent.get("system.memory.mature.bytes")[1]},
        :large => {:bytes => @agent.get("system.memory.large.bytes")[1]},
        :code => {:bytes => @agent.get("system.memory.code.bytes")[1]},
        :symbols => {:bytes => @agent.get("system.memory.symbols.bytes")[1]}
      }
    }

    stats
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
Young   #{sprintf("% 22d", stats[:gc][:young][:count])} #{sprintf("% 16d             ", stats[:gc][:young][:total_wallclock])} #{sprintf("% 16d            ", stats[:gc][:young][:last_wallclock])}
Full    #{sprintf("% 22d", stats[:gc][:full][:count])} #{sprintf("% 16d / % 10d", stats[:gc][:full][:total_stop_wallclock], stats[:gc][:full][:total_concurrent_wallclock])} #{sprintf("% 16d / % 10d", stats[:gc][:full][:last_stop_wallclock], stats[:gc][:full][:last_concurrent_wallclock])}

Allocation
             Objects allocated        Bytes allocated
Young   #{sprintf("% 22d", stats[:memory][:counter][:young_objects])} #{sprintf("% 22d", stats[:memory][:counter][:young_bytes])}
Promoted#{sprintf("% 22d", stats[:memory][:counter][:promoted_objects])} #{sprintf("% 22d", stats[:memory][:counter][:promoted_bytes])}
Mature  #{sprintf("% 22d", stats[:memory][:counter][:mature_objects])} #{sprintf("% 22d", stats[:memory][:counter][:mature_bytes])}


Usage
                    Bytes used
Young   #{sprintf("% 22d", stats[:memory][:young][:bytes])}
Mature  #{sprintf("% 22d", stats[:memory][:mature][:bytes])}
Large   #{sprintf("% 22d", stats[:memory][:large][:bytes])}
Code    #{sprintf("% 22d", stats[:memory][:code][:bytes])}
Symbols #{sprintf("% 22d", stats[:memory][:symbols][:bytes])}
      OUT
    end

    def self.total_time
      (Rubinius::GC.time - @since) / 1000.0
    end

  end

end
