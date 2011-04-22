require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  ary = (0..100).to_a

  x.report "map" do
    ary.map { |x| 1 }
  end

  x.report "map!" do
    ary.dup.map! { |x| 1 }
  end
end
