require 'benchmark'

total = (ENV['TOTAL'] || 1_000).to_i

ary = Array.new(5_000) { |i| i }

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times { |i| i }
  end
  
  x.report("Array#each") do
    total.times { |i| ary.each { |a| a } }
  end
end
