  require 'benchmark'
  require 'benchmark/ips'

  Benchmark.ips do |x|

    s = "abcdefghijk"
    last_index = s.length - 1
    middle_index = last_index/2

    x.report("element assignment at beginning") do |times|
      i = 0
      while i < times
        s[0] = "x"
        i += 1
      end
    end

    x.report("element assignment in middle") do |times|
      i = 0
      while i < times
        s[middle_index] = "x"
        i += 1
      end
    end

    x.report("element assignment at end") do |times|
      i = 0
      while i < times
        s[last_index] = "x"
        i += 1
      end
    end

  end
