require 'benchmark'

s = "0123456789"
S_10_5 = s * 10_000
S_10_6 = s * 100_000
S_10_7 = s * 1_000_000

Benchmark.bm do |x|
  x.report "10^5" do
    S_10_5.to_i
  end

  x.report "10^6" do
    S_10_6.to_i
  end

  x.report "10^7" do
    S_10_7.to_i
  end
end
