require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  char = 98

  x.report "Integer#chr" do |times|
    i = 0
    while i < times
      char.chr
      i += 1
    end
  end

end
