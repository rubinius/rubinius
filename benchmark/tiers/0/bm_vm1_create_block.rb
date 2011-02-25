require 'benchmark'
require 'benchmark/ips'

def return_nil
  nil
end

Benchmark.ips do |x|
  x.report "create block" do |times|
    i=0
    while i < times
      return_nil {}
      i += 1
    end
  end
end
