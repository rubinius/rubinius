require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  part = "aaaa"

  x.report "string interpolation" do |times|
    i = 0
    while i < times
      "#{part} #{part} #{part}"
      i += 1
    end
  end

  x.report "string concat" do |times|
    i = 0
    while i < times
      part + part + part
      i += 1
    end
  end
end
