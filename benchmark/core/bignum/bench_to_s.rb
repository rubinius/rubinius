require 'benchmark'
require 'benchmark/ips'
require 'benchmark/helpers'

Benchmark.ips do |x|

  small_bignum  = Benchmark::Helpers.fixnum_max + 1
  medium_bignum = Benchmark::Helpers.fixnum_max ** 10
  large_bignum  = Benchmark::Helpers.fixnum_max ** 100
  huge_bignum   = Benchmark::Helpers.fixnum_max ** 1000

  x.report "small Bignum#to_s" do |times|
    i = 0
    while i < times
      small_bignum.to_s
      i += 1
    end
  end

  x.report "medium Bignum#to_s" do |times|
    i = 0
    while i < times
      medium_bignum.to_s
      i += 1
    end
  end

  x.report "large Bignum#to_s" do |times|
    i = 0
    while i < times
      large_bignum.to_s
      i += 1
    end
  end

  x.report "huge Bignum#to_s" do |times|
    i = 0
    while i < times
      huge_bignum.to_s
      i += 1
    end
  end

end
