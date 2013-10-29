require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  x.report "(0..10000).step(1)" do |times|
    i = 0
    while i < times
      (0..10000).step(1) { }
      i += 1
    end
  end

  x.report "(0..10000).step(100)" do |times|
    i = 0
    while i < times
      (0..10000).step(100) { }
      i += 1
    end
  end
end
