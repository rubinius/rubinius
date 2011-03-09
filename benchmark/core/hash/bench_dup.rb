require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|

  x.report "dup" do |times|
    i = 0
    while i < times
      $small_hash.dup
      i += 1
    end
  end

end
