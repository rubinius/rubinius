require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_string.rb', __FILE__)

Benchmark.ips do |x|

  short_sentence = $short_sentence.dup
  needle = 98
  string = "the"
  regexp = /[aeiou](.)\1/
  offset = 90

  x.report "index(fixnum)" do |times|
    i = 0
    while i < times
      short_sentence.index(needle)
      i += 1
    end
  end

  x.report "index(fixnum, offset)" do |times|
    i = 0
    while i < times
      short_sentence.index(needle, offset)
      i += 1
    end
  end

  x.report "index(string)" do |times|
    i = 0
    while i < times
      short_sentence.index(string)
      i += 1
    end
  end

  x.report "index(string, offset)" do |times|
    i = 0
    while i < times
      short_sentence.index(string)
      i += 1
    end
  end

  x.report "index(regexp)" do |times|
    i = 0
    while i < times
      short_sentence.index(regexp,0)
      i += 1
    end
  end

  x.report "index(regexp, offset)" do |times|
    i = 0
    while i < times
      short_sentence.index(regexp, offset)
      i += 1
    end
  end

end
