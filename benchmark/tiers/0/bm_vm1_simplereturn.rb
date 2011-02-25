require 'benchmark'
require 'benchmark/ips'

def return_one
  return 1
end

Benchmark.ips do |x|
  x.report "call/return" do |times|
    i = 0
    while i < times
      return_one
      i += 1
    end
  end
end

