require 'benchmark'
require 'benchmark/ips'

c1 = Class.new do
  def m
    1
  end
end

c2 = Class.new do
  def m
    2
  end
end

o1 = c1.new
o2 = c2.new

Benchmark.ips do |x|
  x.report "poly method" do |times|
    which = false
    o = o1

    i = 0
    while i < times
      if which
        o = o1
      else
        o = o2
      end

      which = !which
      o.m; o.m; o.m; o.m; o.m; o.m; o.m; o.m
      i += 1
    end
  end
end
