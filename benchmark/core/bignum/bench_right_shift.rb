require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  x.report "right shift by 1" do |times|
    i = 0
    while i < times
      number = 0xffffffffffffffffffff
      number >> 1
      i += 1
    end
  end

  x.report "right shift by 8" do |times|
    i = 0
    while i < times
      number = 0xffffffffffffffffffff
      number >> 8
      i += 1
    end
  end

  x.report "right shift by 16" do |times|
    i = 0
    while i < times
      number = 0xfffffffffffffffffff
      number >> 16
      i += 1
    end
  end

  x.report "right shift by 32" do |times|
    i = 0
    while i < times
      number = 0xffffffffffffffffffff
      number >> 32
      i += 1
    end
  end

  x.report "right shift by 64" do |times|
    i = 0
    while i < times
      number = 0xffffffffffffffffffff
      number >> 64
      i += 1
    end
  end
end

