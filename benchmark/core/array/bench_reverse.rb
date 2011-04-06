require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_array.rb', __FILE__)

Benchmark.ips do |x|

  large_array = $large_array.dup
  
  x.report "reverse" do |times|
    i = 0
    while i < times
      large_array.reverse
      i += 1
    end
  end

  x.report "reverse!" do |times|
    i = 0
    while i < times
      array = large_array.dup
      array.reverse!
      i += 1
    end
  end
end
