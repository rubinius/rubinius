require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  regular_obj = Object.new
  tainted_obj  = Object.new
  tainted_obj.taint

  x.report "Object#tainted? => false" do |times|
    i = 0
    while i < times
      regular_obj.tainted?
      i += 1
    end
  end

  x.report "Object#tainted? => true" do |times|
    i = 0
    while i < times
      tainted_obj.tainted?
      i += 1
    end
  end

end

