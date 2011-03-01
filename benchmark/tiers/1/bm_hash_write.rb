require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  x.report "hash write" do |times|
    i = 0
    while i < times
      h = {}
      j = 0
      while j < 100
        h[j] = j
        j += 1
      end
      i += 1
    end
  end
end
