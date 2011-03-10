require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
	small_hash = $small_hash
	small_hash2 = small_hash.dup
	different_hash = $different_hash

	x.report "hsh==hsh" do |times|
    i = 0
    while i < times
    	x = small_hash == small_hash
      i += 1
    end
  end

	x.report "hsh==hsh.dup" do |times|
    i = 0
    while i < times
    	x = small_hash == small_hash2
      i += 1
    end
  end

	x.report "hsh==diff_hsh" do |times|
    i = 0
    while i < times
    	x = small_hash == different_hash
      i += 1
    end
  end
end