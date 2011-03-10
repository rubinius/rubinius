require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
  
  small_hash = $small_hash
  
  x.report "each" do |times|
    i = 0
    while i < times
      small_hash.each {|k,v| nil }
      i += 1
    end
  end
  x.report "each_pair" do |times|
    i = 0
    h = {}
    while i < times
      small_hash.each_pair {|k,v| nil }
      i += 1
    end
  end

  x.report "each_key" do |times|
    i = 0
    h = {}
    while i < times
      small_hash.each_key {|k| nil }
      i += 1
    end
  end

  x.report "each_value" do |times|
    i = 0
    h = {}
    while i < times
      small_hash.each_value {|v| nil }
      i += 1
    end
  end
end
