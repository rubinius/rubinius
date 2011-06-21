require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_string.rb', __FILE__)

Benchmark.ips do |x|

  start = 0
  index = 50
  insert = "abc"

  x.report "insert(#{start},'#{insert}')" do |times|
    i = 0
    while i < times
      $short_sentence.dup.insert(start, insert)
      i += 1
    end
  end

  x.report "insert(-1, '#{insert}')" do |times|
    i = 0
    while i < times
      $short_sentence.dup.insert(-1, insert)
      i += 1
    end
  end

  x.report "insert(#{index}, '#{insert}')" do |times|
    i = 0
    while i < times
      $short_sentence.dup.insert(index, insert)
      i += 1
    end
  end

end
