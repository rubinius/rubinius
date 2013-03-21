# Monte Carlo Pi
# Submitted by Seo Sanghyeon


require 'benchmark'
require 'benchmark/ips'

def monte_carlo_pi
  count = 0
  sample = 100_000

  sample.times do
    x = rand
    y = rand
    if x * x + y * y < 1
      count += 1
    end
  end

  4.0 * count / sample
end

Benchmark.ips do |x|
  x.report "monte carlo pi" do
    monte_carlo_pi
  end
end
