# Lucas–Lehmer primality test for Mersenne numbers (2**p - 1)

def is_prime?(p)
  s = 4
  m = 2**p - 1
  (p-2).times do
    s = (s**2 - 2) % m
  end
  s == 0 ? true : false
end

# 4 Mersenne's exponents
Bench.run [9689, 9941, 11213, 19937] do |n|
  puts "2**#{n} - 1 is prime" if is_prime?(n)
end
