require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_array.rb', __FILE__)

Benchmark.ips do |x|
  
  large_array = $large_array.dup

  x.report "reject all" do |times|
    i = 0
    while i < times
      large_array.reject { |v| true }
      i += 1
    end
  end

  x.report "reject none" do |times|
    i = 0
    while i < times
      large_array.reject { |v| false }
      i += 1
    end
  end

  x.report "reject! all" do |times|
    i = 0
    while i < times
      large_array.reject! { |v| true }
      i += 1
    end
  end

  x.report "reject! none" do |times|
    i = 0
    while i < times
      large_array.reject! { |v| false }
      i += 1
    end
  end
end
