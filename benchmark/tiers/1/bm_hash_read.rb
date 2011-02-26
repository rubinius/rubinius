require 'benchmark'
require 'benchmark/ips'

h = { 47 => :one }

Benchmark.ips do |x|
  x.report "hash read" do
    100.times { |i| h[i] }
  end

  x.report "hash update" do
    100.times { h[47] = :two }
  end
end
