require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  x.report "array append" do |times|
    ary = []
    i = 0
    while i < times
      j = 0
      while j < 100
        ary << j
        j += 1
      end

      i += 1
    end
  end
end

