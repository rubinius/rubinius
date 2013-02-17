require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  small_float = -10000.0

  x.report "- Float" do |times|
    i = 0
    while i < times
      -small_float
      i += 1
    end
  end

end
