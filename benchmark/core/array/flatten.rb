require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  flat_array = (0...50).to_a

  nested_array = (0...5).to_a
  3.times do
    nested_array = (0...5).map { nested_array }
  end

  x.report "array flatten - flat" do
    flat_array.flatten
  end

  x.report "array flatten! - flat" do
    flat_array.dup.flatten!
  end

  x.report "array flatten - nested" do
    nested_array.flatten
  end

  x.report "array flatten! - nested" do
    nested_array.dup.flatten!
  end
end
