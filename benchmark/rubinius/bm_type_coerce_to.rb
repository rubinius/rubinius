require 'benchmark'

total = (ENV['TOTAL'] || 1_000_000).to_i

fixnums = Array.new(total).fill { |a| rand(100_000) }

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times { |i| fixnums[i] }
  end

  x.report("Type.coerce_to(obj, Integer, :to_int)") do
    total.times { |i| Type.coerce_to fixnums[i], Integer, :to_int }
  end

  x.report("obj.kind_of? Fixnum") do
    total.times { |i| fixnums[i].kind_of? Fixnum }
  end

  x.report("obj.__kind_of__ Fixnum") do
    total.times { |i| fixnums[i].__kind_of__ Fixnum }
  end
end
