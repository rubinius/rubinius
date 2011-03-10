require 'benchmark'
require 'benchmark/ips'
require 'time'

Benchmark.ips do |x|
  int_second = Time.parse("2011-03-08T10:09:08.467").to_i
  float_second = Time.parse("2011-03-08T10:09:08.467").to_f
  

  x.report "#at with integer" do |times|
    i = 0
    while i < times
      Time.at(int_second)
      i += 1
    end
  end

  x.report "#at with float" do |times|
    i = 0
    while i < times
      Time.at(float_second)
      i += 1
    end
  end

end
