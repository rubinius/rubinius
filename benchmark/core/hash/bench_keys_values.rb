require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|

  x.report "#keys" do |times|
    i = 0
    while i < times
      $small_hash.keys
      i += 1
    end
  end

  x.report "#values" do |times|
    i = 0
    while i < times
      $small_hash.values
      i += 1
    end
  end

end
