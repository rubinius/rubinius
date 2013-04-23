require 'benchmark'
require 'benchmark/ips'

class Bar
  def bar
  end
end

Benchmark.ips do |x|

  obj = Bar.new

  x.report "Object#respond_to?(:bar) => true" do |times|
    i = 0
    while i < times
      obj.respond_to?(:bar)
      i += 1
    end
  end

  x.report "Object#respond_to?(:foo) => false" do |times|

    i = 0
    while i < times
      obj.respond_to?(:foo)
      i += 1
    end
  end

end


