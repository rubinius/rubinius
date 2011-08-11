require 'benchmark'
require 'benchmark/ips'

require File.expand_path('../setup', __FILE__)

Benchmark.ips do |x|
  x.report "new" do |t|
    i = 0
    while i < t
      h = new_hash
      i += 1
    end
  end

  x.report "allocate" do |t|
    i = 0
    while i < t
      h = hash_class.allocate
      i += 1
    end
  end
end
