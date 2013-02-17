require 'benchmark'
require 'benchmark/ips'
require 'benchmark/helpers'

Benchmark.ips do |x|

  tiny_float  = 9.0
  small_float = 10000.0
  fixnum      = 1234

  x.report "Float ** Float => Float" do |times|
    i = 0
    while i < times
      tiny_float ** small_float
      i += 1
    end
  end

  x.report "Float ** Fixnum => Float" do |times|
    i = 0
    while i < times
      small_float ** fixnum
      i += 1
    end
  end

end
