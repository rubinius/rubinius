require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  strings = {:a => 'a', :b => 'b', :c => 'c', :d => 'd', :e => 'e', :f => 'f', :g => 'g', :h => 'h', :i => 'i', :j => 'j'}
  numbers = {:a => -4, :b => -81, :c => 0, :d => 5, :e => 12, :f => -1_000_000, :g => 1, :h => 10, :i => 100, :j => 1000}

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
