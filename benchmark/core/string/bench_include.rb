require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_string.rb', __FILE__)

Benchmark.ips do |x|
  short_sentence = $short_sentence.dup

  x.report "include? matches" do |times|
    i = 0
    while i < times
      short_sentence.include?('Lend')
      i += 1
    end
  end

  x.report "include? no matches" do |times|
    i = 0
    while i < times
      short_sentence.include?('nOpe')
      i += 1
    end
  end
end
