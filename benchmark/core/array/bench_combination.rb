require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_array.rb', __FILE__)

Benchmark.ips do |x|
  small_array = $small_array.dup

  x.report "combination by groups of 2" do |times|
    i = 0
    while i < times
      small_array.combination(2).to_a
      i += 1
    end
  end

  x.report "combination by groups of 4" do |times|
    i = 0
    while i < times
      small_array.combination(4).to_a
      i += 1
    end
  end

end
