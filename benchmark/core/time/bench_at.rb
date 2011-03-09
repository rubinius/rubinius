require 'benchmark'
require 'benchmark/ips'
require 'time'

Benchmark.ips do |x|
  nearest_second = Time.parse("2011-03-08T10:09:08.467").to_i
  fractional_second = Time.parse("2011-03-08T10:09:08.467").to_f
  

  x.report "#at to nearest second" do |times|
    i = 0
    while i < times
      Time.at(nearest_second)
      i += 1
    end
  end

  x.report "#at for fractional second" do |times|
    i = 0
    while i < times
      Time.at(fractional_second)
      i += 1
    end
  end

end
