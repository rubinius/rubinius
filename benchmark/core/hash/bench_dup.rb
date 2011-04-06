require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
  
  small_hash = $small_hash
  
  x.report "dup" do |times|
    i = 0
    while i < times
      small_hash.dup
      i += 1
    end
  end

  x.report "replace" do |times|
    i = 0
    h = {}
    while i < times
      h.replace small_hash
      i += 1
    end
  end

end
