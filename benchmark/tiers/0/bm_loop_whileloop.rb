require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  x.report "while loop" do |times|
    i = 0
    while i < times
      i += 1
    end
  end
end
