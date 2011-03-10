require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
  
  small_hash = $small_hash
  keys = small_hash.keys
  
  x.report "key? all match" do |times|
    i = 0
    while i < times
      keys.each { |key| small_hash.key?(key) }
      i += 1
    end
  end

  x.report "key? never match" do |times|
    i = 0
    while i < times
      keys.each { |key| small_hash.key?(nil) }
      i += 1
    end
  end

end
