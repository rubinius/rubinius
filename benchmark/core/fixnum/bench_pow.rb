require 'benchmark'
require 'benchmark/ips'
require 'benchmark/helpers'

Benchmark.ips do |x|

  tiny_fixnum  = 9
  small_fixnum = 10000
  float        = 1234.0

  x.report "Fixnum ** Fixnum => Fixnum" do |times|
    i = 0
    while i < times
      tiny_fixnum ** tiny_fixnum
      i += 1
    end
  end

  x.report "Fixnum ** Fixnum => Bignum" do |times|
    i = 0
    while i < times
      small_fixnum ** tiny_fixnum
      i += 1
    end
  end

  x.report "Fixnum ** Float => Float" do |times|
    i = 0
    while i < times
      tiny_fixnum ** float
      i += 1
    end
  end
end
