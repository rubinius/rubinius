require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  x.report "null", " #nothing "
end
