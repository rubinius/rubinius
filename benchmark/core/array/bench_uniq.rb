require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_array.rb', __FILE__)

Benchmark.ips do |x|
  large_array = $large_array.dup

  x.report "uniq" do |times|
    i = 0
    while i < times
      large_array.uniq
      i += 1
    end
  end

  x.report "uniq!" do |times|
    i = 0
    while i < times
      large_array = $large_array.dup
      large_array.uniq!
      i += 1
    end
  end

end
