# encoding: utf-8

require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_string.rb', __FILE__)

Benchmark.ips do |x|

  base = "hellü 椎名深夏"
  string = base * 1_000

  x.report "String#size" do |times|
    i = 0
    while i < times
      str = string.dup << "a"
      str.size
      i += 1
    end
  end

end

