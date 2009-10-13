# Lucas–Lehmer primality test for Mersenne numbers (2**p - 1)

def is_prime?(p)
  s = 4
  m = 2**p - 1
  (p-2).times do
    s = (s**2 - 2) % m
  end
  s == 0 ? true : false
end

# [9689, 9941, 11213, 19937].each { |n| is_prime?(n) }
# 4 Mersenne's exponents
def Bench.run
  is_prime?(9689)
end
