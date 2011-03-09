require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  small_fixnum = -10000

  x.report "- Fixnum" do |times|
    i = 0
    while i < times
      -small_fixnum
      i += 1
    end
  end

end
