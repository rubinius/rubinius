require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  part = "aaaa"

  x.report "string concat" do |times|
    i = 0
    while i < times
      "#{part} #{part} #{part}"
    end
  end
end
