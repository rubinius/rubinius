require 'benchmark'
require 'benchmark/ips'
require 'benchmark/helpers'

Benchmark.ips do |x|

  small_float = 10000.0
  big_float   = Benchmark::Helpers.fixnum_max.to_f
  bignum      = Benchmark::Helpers.fixnum_max + 1
  fixnum      = 1234

  x.report "Float % Float => Float" do |times|
    i = 0
    while i < times
      big_float % small_float
      i += 1
    end
  end

  x.report "Float % Fixnum => Float" do |times|
    i = 0
    while i < times
      big_float % fixnum
      i += 1
    end
  end

  x.report "Float % Bignum => Float" do |times|
    i = 0
    while i < times
      small_float % bignum
      i += 1
    end
  end
end
