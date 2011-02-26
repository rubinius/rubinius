require 'benchmark'
require 'benchmark/ips'

c = Class.new do
  def m(a)
    1
  end
end

cc = Class.new(c) do
  def m(a)
    super
  end
end

Benchmark.ips do |x|
  obj = cc.new

  x.report "zsuper" do |times|
    i = 0
    while i < times
      obj.m 10
      i+=1
    end
  end
end
