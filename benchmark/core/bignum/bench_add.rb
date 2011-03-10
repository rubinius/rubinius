require 'benchmark'
require 'benchmark/ips'
require 'benchmark/helpers'

Benchmark.ips do |x|

  fixnum        = Benchmark::Helpers.fixnum_max
  small_bignum  = Benchmark::Helpers.fixnum_max + 1
  medium_bignum = Benchmark::Helpers.fixnum_max ** 10
  large_bignum  = Benchmark::Helpers.fixnum_max ** 100
  float         = 12345.3 ** 34

  x.report "small Bignum + Bignum" do |times|
    i = 0
    while i < times
      small_bignum + small_bignum
      i += 1
    end
  end

  x.report "medium Bignum + Bignum" do |times|
    i = 0
    while i < times
      medium_bignum + medium_bignum
      i += 1
    end
  end

  x.report "large Bignum + Bignum" do |times|
    i = 0
    while i < times
      large_bignum + large_bignum
      i += 1
    end
  end

  x.report "Bignum + Fixnum" do |times|
    i = 0
    while i < times
      medium_bignum + fixnum
      i += 1
    end
  end

  x.report "Bignum + Float" do |times|
    i = 0
    while i < times
      medium_bignum + float
      i += 1
    end

  end

end
