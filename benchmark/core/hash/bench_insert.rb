require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
	small_hash = $small_hash

	x.report "hsh[k]=v (already exists)" do |times|
    i = 0
    while i < times
      small_hash[:SlBkyplxcZ] = i
      i += 1
    end
  end

	x.report "hsh[k]=v (new item)" do |times|
    i = 0
    while i < times
      h = Hash.new
      h[i] = i
      i += 1
    end
  end

  [10, 100, 1000, 10000].each do |count|
  	x.report "hsh[i]=i #{count} times" do |times|
      i = 0
      while i < times
        h = Hash.new
        j = 0
        while j < count
          h[j] = i
          j += 1
        end
        i += 1
      end
    end
  end
end