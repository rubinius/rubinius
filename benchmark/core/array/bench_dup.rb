require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  strings = ('a'..'j').to_a
  numbers = [-4, -81, 0, 5, 12, -1_000_000, 1, 10, 100, 1000]

  x.report "dup strings" do |times|
    i = 0
    while i < times
      strings.dup
      i += 1
    end
  end

  x.report "dup numbers" do |times|
    i = 0
    while i < times
      numbers.dup
      i += 1
    end
  end
end
