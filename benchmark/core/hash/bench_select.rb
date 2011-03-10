require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
  
  small_hash = $small_hash
  
  x.report "select all" do |times|
    i = 0
    while i < times
      small_hash.select { |k,v| true }
      i += 1
    end
  end

  x.report "select none" do |times|
    i = 0
    while i < times
      small_hash.select { |k,v| false }
      i += 1
    end
  end

end
