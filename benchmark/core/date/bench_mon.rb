  require 'benchmark'
  require 'benchmark/ips'
  require 'date'

  fi = Date.new(1926,6,1)

  Benchmark.ips do |x|

    x.report "Date#mon " do |times|
      i = 0
      while i < times
        (fi + i%2).mon #diddle to avoid memoization
        i += 1
      end
    end

    x.report "Date#year" do |times|
      i = 0
      while i < times
        (fi + i%2).year #diddle to avoid memoization
        i += 1
      end
    end

  end
