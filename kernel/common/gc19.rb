# -*- encoding: us-ascii -*-

module GC
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
          :total_wallclock => @agent.get('system.gc.full.total_wallclock')[1],
          :last_wallclock => @agent.get('system.gc.full.last_wallclock')[1]
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
end
