require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  regular_obj = Object.new
  frozen_obj  = Object.new
  frozen_obj.freeze

  x.report "Object#frozen? => false" do |times|
    i = 0
    while i < times
      regular_obj.frozen?
      i += 1
    end
  end

  x.report "Object#frozen? => true" do |times|
    i = 0
    while i < times
      frozen_obj.frozen?
      i += 1
    end
  end

end

