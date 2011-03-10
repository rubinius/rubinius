require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_string.rb', __FILE__)

Benchmark.ips do |x|
  
  short_sentence = $short_sentence.dup
  range = 34..40
  regexp = /[aeiou](.)\1/

  x.report "slice(fixnum)" do |times|
    i = 0
    while i < times
      short_sentence.slice(34)
      i += 1
    end
  end

  x.report "slice(fixnum, fixnum)" do |times|
    i = 0
    while i < times
      short_sentence.slice(34, 40)
      i += 1
    end
  end

  x.report "slice.(range)" do |times|
    i = 0
    while i < times
      short_sentence.slice(range)
      i += 1
    end
  end

  x.report "slice(regex)" do |times|
    i = 0
    while i < times
      short_sentence.slice(regexp)
      i += 1
    end
  end

  x.report "slice(regexp, 1)" do |times|
    i = 0
    while i < times
      short_sentence.slice(regexp,1 )
      i += 1
    end
  end

  x.report "slice(other_str), matches" do |times|
    i = 0
    while i < times
      short_sentence.slice('winter')
      i += 1
    end
  end

  x.report "slice(other_str), no matches" do |times|
    i = 0
    while i < times
      short_sentence.slice('nOpe')
      i += 1
    end
  end
end
