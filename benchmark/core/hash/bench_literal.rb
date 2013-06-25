require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
	x.report "{} literal" do |times|
    i = 0
    while i < times
      x = {}
      i += 1
    end
  end
end
