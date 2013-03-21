require 'benchmark'
require 'benchmark/ips'

h = { 47 => :one }

Benchmark.ips do |x|
  x.report "hash read" do |times|
    i = 0
    while i < times
      h[i]
      i += 1
    end
  end

  x.report "hash update" do |times|
    i = 0
    while i < times
      h[47] = :two
      i += 1
    end
  end
end
