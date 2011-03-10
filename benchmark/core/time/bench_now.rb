require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  x.report "Time#now" do |times|
    i = 0
    while i < times
      Time.now
      i += 1
    end
  end

end
