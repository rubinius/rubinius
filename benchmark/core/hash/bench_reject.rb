require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
  
  small_hash = $small_hash
  
  x.report "reject all" do |times|
    i = 0
    while i < times
      small_hash.reject { |k,v| true }
      i += 1
    end
  end

  x.report "reject none" do |times|
    i = 0
    while i < times
      small_hash.reject { |k,v| false }
      i += 1
    end
  end

  x.report "reject! all" do |times|
    i = 0
    while i < times
      hash = small_hash.dup # must dup since #reject! modifies receiver
      hash.reject! { |k,v| true }
      i += 1
    end
  end

  x.report "reject! none" do |times|
    i = 0
    while i < times
      small_hash.reject! { |k,v| false }
      i += 1
    end
  end
end
