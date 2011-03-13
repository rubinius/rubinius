require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_string.rb', __FILE__)

Benchmark.ips do |x|

  short_string = $short_sentence

  x.report "reverse" do |times|
    i = 0
    while i < times
      short_string.reverse
      i += 1
    end
  end

  x.report "reverse!" do |times|
    i = 0
    while i < times
      str = short_string.dup
      str.reverse!
      i += 1
    end
  end
end
