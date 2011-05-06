require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  ary = [0]

  x.report "little array" do
    ary * 10
  end

  x.report "medium array" do
    ary * 100
  end

  x.report "large array" do
    ary * 1000
  end

  x.report "giant array" do
    ary * 10000
  end
end
