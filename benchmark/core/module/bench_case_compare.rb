require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  fixnum       = 1
  string       = ""

  x.report "Fixnum#=== with a Fixnum" do |times|
    i = 0
    while i < times
      Fixnum === fixnum
      i += 1
    end
  end

  x.report "Fixnum#=== with a String" do |times|
    i = 0
    while i < times
      Fixnum === string
      i += 1
    end
  end

  x.report "String#=== with a Fixnum" do |times|
    i = 0
    while i < times
      String === fixnum
      i += 1
    end
  end

  x.report "String#=== with a String" do |times|
    i = 0
    while i < times
      String === string
      i += 1
    end
  end

end

