require 'benchmark'
require 'benchmark/ips'
require 'benchmark/helpers'
require 'rational'

Benchmark.ips do |x|

  small_fixnum = 10000
  big_fixnum   = Benchmark::Helpers.fixnum_max

  x.report "Fixnum gcd Fixnum => Fixnum" do |times|
    i = 0
    while i < times
      big_fixnum.gcd(small_fixnum)
      i += 1
    end
  end

end
