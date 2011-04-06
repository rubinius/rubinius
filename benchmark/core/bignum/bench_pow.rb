require 'benchmark'
require 'benchmark/ips'
require 'benchmark/helpers'

Benchmark.ips do |x|

  tiny_fixnum  = 9
  small_bignum = Benchmark::Helpers.fixnum_max + 1
  float        = 12.0

  x.report "Bignum ** Fixnum" do |times|
    i = 0
    while i < times
      small_bignum ** tiny_fixnum
      i += 1
    end
  end

  x.report "Bignum ** Float" do |times|
    i = 0
    while i < times
      small_bignum ** float
      i += 1
    end
  end
end
