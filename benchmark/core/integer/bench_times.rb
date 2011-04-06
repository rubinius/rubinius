require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  x.report "Integer#times" do |times|
    i = 0
    while i < times
      1000.times{ |x| }
      i += 1
    end
  end
  
end
