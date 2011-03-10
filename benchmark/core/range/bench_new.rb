require 'benchmark'
require 'benchmark/ips'
#require File.expand_path('../shared_string.rb', __FILE__)

Benchmark.ips do |x|
  #short_sentence = $short_sentence.dup

  x.report "0..10" do |times|
    i = 0
    while i < times
      0..10
      i += 1
    end
  end

  x.report "0...10" do |times|
    i = 0
    while i < times
      0...10
      i += 1
    end
  end

  x.report "0xffff..0xfffff" do |times|
    i = 0
    while i < times
      0xffff..0xfffff
      i += 1
    end
  end

  x.report "0xffff...0xfffff" do |times|
    i = 0
    while i < times
      0xffff...0xfffff
      i += 1
    end
  end

  x.report "0.5..2.4" do |times|
    i = 0
    while i < times
      0.5..2.4
      i += 1
    end
  end

  x.report "0.5...2.4" do |times|
    i = 0
    while i < times
      0.5...2.4
      i += 1
    end
  end

  x.report "'a'..'j'" do |times|
    i = 0
    while i < times
      'a'..'j'
      i += 1
    end
  end

  x.report "'a'...'j'" do |times|
    i = 0
    while i < times
      'a'...'j'
      i += 1
    end
  end
end
