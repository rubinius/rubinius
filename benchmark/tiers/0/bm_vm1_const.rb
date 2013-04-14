require 'benchmark'
require 'benchmark/ips'

Const = 1

Benchmark.ips do |x|
  x.report "const read" do |times|
    i = 0
    while i < times
      Const
      i += 1
    end
  end
end
