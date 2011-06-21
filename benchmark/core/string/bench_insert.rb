require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_string.rb', __FILE__)

Benchmark.ips do |x|

  short_sentence = $short_sentence.dup
  start = 0
  index = 50
  insert = "abc"

  x.report "insert(#{start},'#{insert}')" do |times|
    i = 0
    while i < times
      short_sentence.insert(start, insert)
      i += 1
    end
  end

  x.report "insert(-1, '#{insert}')" do |times|
    i = 0
    while i < times
      short_sentence.insert(-1, insert)
      i += 1
    end
  end

  x.report "insert(#{index}, '#{insert}')" do |times|
    i = 0
    while i < times
      short_sentence.insert(index, insert)
      i += 1
    end
  end

end
