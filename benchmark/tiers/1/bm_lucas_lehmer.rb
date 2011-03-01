require 'benchmark'
require 'benchmark/ips'

# Lucas–Lehmer primality test for Mersenne numbers (2**p - 1)

# [9689, 9941, 11213, 19937].each { |n| is_prime?(n) }
# 4 Mersenne's exponents
#
# MEASUREMENT NOTE:
# 
# Because of the 2**p, this becomes mostly a Bignum benchmark.

Benchmark.ips do |x|
  x.report "prime test" do
    p = 1001
    s = 4
    m = 2**p - 1
    (p-2).times do
      s = (s**2 - 2) % m
    end
    s == 0 ? true : false
  end
end
