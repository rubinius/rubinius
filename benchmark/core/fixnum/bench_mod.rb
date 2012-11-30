require 'benchmark'
require 'benchmark/ips'
require 'benchmark/helpers'

Benchmark.ips do |x|

  small_fixnum = 10000
  big_fixnum   = Benchmark::Helpers.fixnum_max
  bignum       = Benchmark::Helpers.fixnum_max + 1
  float        = 1234.0

  x.report "Fixnum % Fixnum => Fixnum" do |times|
    i = 0
    while i < times
      big_fixnum % small_fixnum
      i += 1
    end
  end

  x.report "Fixnum % Bignum => Fixnum" do |times|
    i = 0
    while i < times
      big_fixnum % bignum
      i += 1
    end
  end

  x.report "Fixnum % Float => Float" do |times|
    i = 0
    while i < times
      small_fixnum % float
      i += 1
    end
  end
end
