require 'benchmark'
require 'benchmark/ips'

h = { :a => 1, :b => 2, :c => 3, :d => 4, :e => 5, :f => 6 }
all = h.keys

Benchmark.ips do |x|
  x.report "values_at 1" do
    h.values_at :a
  end

  x.report "values_at 2" do
    h.values_at :a, :b
  end

  x.report "values_at 3" do
    h.values_at :a, :b, :c
  end

  x.report "values_at 6" do
    h.values_at(*all)
  end
end
