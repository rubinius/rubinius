require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|

  regular_obj = Object.new
  untrusted_obj  = Object.new
  untrusted_obj.untrust

  x.report "Object#untrusted? => false" do |times|
    i = 0
    while i < times
      regular_obj.untrusted?
      i += 1
    end
  end

  x.report "Object#frozen? => true" do |times|
    i = 0
    while i < times
      untrusted_obj.untrusted?
      i += 1
    end
  end

end

