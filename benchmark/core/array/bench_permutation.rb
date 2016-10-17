require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_array.rb', __FILE__)

Benchmark.ips do |x|
  small_array = $small_array.dup

  x.report "permutation by groups of 2" do |times|
    i = 0
    while i < times
      small_array.permutation(2)
      i += 1
    end
  end

end
