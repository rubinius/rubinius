require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  x.report "Integer#downto" do |times|
    i = 0
    while i < times
      10000.downto(1)
      i += 1
    end
  end

  x.report "Integer#upto" do |times|
    i = 0
    while i < times
      1.upto(10000)
      i += 1
    end
  end

end
