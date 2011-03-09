require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  hash_strings = {:a => 'a', :b => 'b', :c => 'c', :d => 'd', :e => 'e', :f => 'f', :g => 'g', :h => 'h', :i => 'i', :j => 'j'}
  hash_numbers = {:a => -4, :b => -81, :c => 0, :d => 5, :e => 12, :f => -1_000_000, :g => 1, :h => 10, :i => 100, :j => 1000}

  x.report "reject string" do |times|
    i = 0
    while i < times
      h = hash_strings.dup
      h.reject { |k,v| v < 'd' }
      i += 1
    end
  end

  x.report "reject numbers" do |times|
    i = 0
    while i < times
      h = hash_numbers.dup
      h.reject { |k,v| v < 10 }
      i += 1
    end
  end

  x.report "reject! string" do |times|
    i = 0
    while i < times
      h = hash_strings.dup
      h.reject! { |k,v| v < 'd' }
      i += 1
    end
  end

  x.report "reject! numbers" do |times|
    i = 0
    while i < times
      h = hash_numbers.dup
      h.reject! { |k,v| v < 10 }
      i += 1
    end
  end
end
