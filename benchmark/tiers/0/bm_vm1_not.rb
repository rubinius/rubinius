require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  obj = Object.new

  x.report "not" do |times|
    i = 0
    while i < times
      !obj
      i += 1
    end
  end
end
