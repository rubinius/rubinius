require 'benchmark'
require 'benchmark/ips'

# The Computer Language Shootout
# http://shootout.alioth.debian.org/
#
# contributed by Glenn Parker, March 2005
# modified by Evan Phoenix, Sept 2006

def sieve(m)
  flags = Flags.dup[0,m]
  count = 0
  pmax = m - 1
  p = 2
  while p <= pmax
    unless flags[p].zero?
      count += 1
      mult = p
      while mult <= pmax
        flags[mult] = 0
        mult += p
      end
    end
    p += 1
  end
  count
end


def sieve_iterations(iterations)
  m = iterations
  iterations.downto(iterations-2) do |exponent|
    break if exponent < 0
    m = (1 << exponent) * 1_000
    # m = (2 ** exponent) * 10_000
    count = sieve(m)
  end
end

iterations = 7
Flags = [1] * (2 ** iterations * 10_000)

Benchmark.ips do |x|
  x.report "nsieve" do
    sieve_iterations(iterations)
  end
end
