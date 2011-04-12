require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  x.compare!

  x.report "x ||= y (false)" do |times|
    x = false
    y = false
    i = 0
    while i < times
      x ||= y
      i += 1
    end
  end

  x.report "x |= y (false)" do |times|
    x = false
    y = false
    i = 0
    while i < times
      x |= y
      i += 1
    end
  end

  x.report "x ||= y (true)" do |times|
    x = true
    y = true
    i = 0
    while i < times
      x ||= y
      i += 1
    end
  end

  x.report "x |= y (true)" do |times|
    x = true
    y = true
    i = 0
    while i < times
      x |= y
      i += 1
    end
  end
end
