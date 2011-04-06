require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
  
  small_hash = $small_hash
  different_hash = $different_hash
  
  x.report "merge" do |times|
    i = 0
    while i < times
      small_hash.merge(different_hash)
      i += 1
    end
  end

  x.report "merge {}" do |times|
    i = 0
    while i < times
      small_hash.merge(different_hash) {|k,o,n| n }
      i += 1
    end
  end

  x.report "merge!" do |times|
    i = 0
    while i < times
      hash = small_hash.dup
      hash.merge!(different_hash)
      i += 1
    end
  end

  x.report "merge! {}" do |times|
    i = 0
    while i < times
      hash = small_hash.dup
      hash.merge!(different_hash) {|k,o,n| n }
      i += 1
    end
  end
end
