require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  obj1 = Object.new
  obj2 = Object.new

  x.report "not equal" do |times|
    i = 0
    while i < times
      obj1 != obj2
      i += 1
    end
  end
end
