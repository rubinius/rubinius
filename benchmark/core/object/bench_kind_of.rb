require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  x.report "Fixnum#kind_of? => true" do |times|
    i = 0
    while i < times
      1.kind_of? Fixnum
      i += 1
    end
  end

  x.report "Fixnum#kind_of? => false" do |times|
    i = 0
    while i < times
      1.kind_of? String
      i += 1
    end
  end

  x.report "String#kind_of? => true" do |times|
    i = 0
    while i < times
      "str".kind_of? String
      i += 1
    end
  end

  x.report "String#kind_of? => false" do |times|
    i = 0
    while i < times
      "str".kind_of? Fixnum
      i += 1
    end
  end

end
