require 'benchmark'
require 'benchmark/ips'
require 'benchmark/helpers'

Benchmark.ips do |x|

  gatherer = []
  x.report "20% survivor rate" do |times|
    i = 0
    while i < times
      obj = Object.new
      if i % 5 == 0
        gatherer << Object.new
      end
      i += 1
    end
  end
end

require 'rubinius/agent'
agent = Rubinius::Agent.loopback
young_size  = agent.get('system.memory.young.bytes').last
mature_size = agent.get('system.memory.mature.bytes').last
large_size  = agent.get('system.memory.large.bytes').last
code_size   = agent.get('system.memory.code.bytes').last
symbol_size = agent.get('system.memory.symbols.bytes').last

puts "Young  GC size:  #{young_size / 1024}kB"
puts "Young  GC count: #{agent.get('system.gc.young.count').last}"
puts "Young  GC time:  #{agent.get('system.gc.young.total_wallclock').last}ms"

puts "Mature GC size:  #{mature_size / 1024}kB"
puts "Mature GC count: #{agent.get('system.gc.full.count').last}"
puts "Mature GC time:  #{agent.get('system.gc.full.total_stop_wallclock').last}ms"
puts "Young  GC waste: #{(young_size.to_f / 2) / (young_size + mature_size + large_size + code_size + symbol_size) * 100}%"
