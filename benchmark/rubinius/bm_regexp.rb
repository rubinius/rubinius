require "benchmark"

MAX = (ENV['TOTAL'] || 1_000).to_i

TEMPLATE = "r(n]sp &xq\nhn^kj)rs\nb c6{lh|4c@jcb [v8\nPvu}s%wijh# lz! \\d\"y7hlKlR nzqxg"

Benchmark.bmbm do |x|
  [1,8,16].each do |mult|
    string = TEMPLATE * mult
    x.report("loop for #{mult}") do
      MAX.times { string }
    end

    x.report("Regexp.escape TEMPLATE * #{mult}") do
      MAX.times { Regexp.escape string }
    end
  end
end
