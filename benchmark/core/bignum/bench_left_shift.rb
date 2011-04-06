require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  x.report "left shift by 1" do |times|
    i = 0
    while i < times
      number = 0xffffffffffffffff
      number << 1
      i += 1
    end
  end

  x.report "left shift by 8" do |times|
    i = 0
    while i < times
      number = 0xffffffffffffffff
      number << 8
      i += 1
    end
  end

  x.report "left shift by 16" do |times|
    i = 0
    while i < times
      number = 0xffffffffffffffff
      number << 16
      i += 1
    end
  end

  x.report "left shift by 32" do |times|
    i = 0
    while i < times
      number = 0xffffffffffffffff
      number << 32
      i += 1
    end
  end

  x.report "left shift by 64" do |times|
    i = 0
    while i < times
      number = 0xffffffffffffffff
      number << 64
      i += 1
    end
  end
end
