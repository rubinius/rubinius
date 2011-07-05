require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_array.rb', __FILE__)

Benchmark.ips do |x|
  small_array = $small_array.dup

  x.report "Array#slice index" do |times|
    i = 0
    while i < times
      small_array.slice(5)
      i += 1
    end
  end

  x.report "Array#slice start, length" do |times|
    i = 0
    while i < times
      small_array.slice(5, 5)
      i += 1
    end
  end

  x.report "Array#slice range" do |times|
    i = 0
    while i < times
      small_array.slice(5..10)
      i += 1
    end
  end

  x.report "Array#slice! index" do |times|
    i = 0
    while i < times
      small_array2 = small_array.dup
      small_array2.slice!(5)
      i += 1
    end
  end

  x.report "Array#slice! start, length" do |times|
    i = 0
    while i < times
      small_array2 = small_array.dup
      small_array2.slice!(5, 5)
      i += 1
    end
  end

  x.report "Array#slice! range" do |times|
    i = 0
    while i < times
      small_array2 = small_array.dup
      small_array2.slice!(5..10)
      i += 1
    end
  end

end
