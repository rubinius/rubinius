# encoding: utf-8

require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  base = "hellü 椎名深夏"

  x.report "String#*" do |times|
    i = 0
    while i < times
      base * 1000
      i += 1
    end
  end

end

