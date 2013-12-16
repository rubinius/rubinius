  require 'benchmark'
  require 'benchmark/ips'

  Benchmark.ips do |x|

    a = 1234567890123456789012345678901234567890
    b = 9876543210987654321098765432109876543210

    x.report "Bignum and Fixnum" do |times|
      i = 0
      while i < times
         c = a & 27
         i += 1
      end
    end

    x.report "Bignum and Bignum" do |times|
      i = 0
      while i < times
         c = a & b
         i += 1
      end
    end

  end
