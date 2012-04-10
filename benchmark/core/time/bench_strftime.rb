require 'benchmark'
require 'benchmark/ips'
require 'time'

Benchmark.ips do |x|
  x.report "#strftime short string" do |times|
    i = 0
    while i < times
      Time.now.strftime('Printed on %m/%d/%Y')
      i += 1
    end
  end

  x.report "#strftime large string" do |times|
    i = 0
    while i < times
      Time.now.strftime('Printed on %m/%d/%Y' * 1000)
      i += 1
    end
  end
end
