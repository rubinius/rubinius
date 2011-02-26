require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  x.report "create array" do |times|
    i = 0
    while i < times
      a = [1,2,3,4,5,6,7,8,9,10]
      i += 1
    end
  end
end
