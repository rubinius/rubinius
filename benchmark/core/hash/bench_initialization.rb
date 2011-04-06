require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|
	small_hash = $small_hash

	x.report "Hash.new empty" do |times|
    i = 0
    while i < times
      x = Hash.new
      i += 1
    end
  end

  [10, 100, 1000].each do |count|
	  array = (1..count*2).to_a # key/value

	  x.report "Hash[#{count} items]" do |times|
	  	i = 0
	  	while i < times
	  		# Note: Poor performance on this may indicate poor performance of the
	  		# splat operator, not Hash's initialization. I don't see
	  		# a way around that, though.
	  		x = Hash[*array]
	  		i += 1
	  	end
	  end
	end
end