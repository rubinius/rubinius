require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_hash.rb', __FILE__)

Benchmark.ips do |x|

  x.report "hsh.reject" do |times|
    i = 0
    while i < times
      $small_hash.reject { |k,v| v.even?}
      i += 1
    end
  end

  x.report "hsh.reject!" do |times|
    i = 0
    while i < times
      small_hash = $small_hash.dup
      small_hash.reject! { |k,v| v.even?}
      i += 1
    end
  end

end
