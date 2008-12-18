require "benchmark"

MAX = (ENV['TOTAL'] || 1_000).to_i

STRING = "r(n]sp &xq\nhn^kj)rs\nb c6{lh|4c@jcb [v8\nPvu}s%wijh# lz! \\d\"y7hlKlR nzqxg"

Benchmark.bmbm do |x|
  x.report("loop") do
    MAX.times { STRING }
  end

  x.report("Regexp.escape") do
    MAX.times { Regexp.escape STRING }
  end
end
