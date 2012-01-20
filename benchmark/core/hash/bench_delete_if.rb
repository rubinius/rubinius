require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|

  x.report "hsh.delete_if" do |times|
    i = 0
    while i < times
      small_hash = $small_hash.dup
      small_hash.delete_if { |k,v| v.even? }
      i += 1
    end
  end

end
