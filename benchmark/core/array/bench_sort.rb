require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_array.rb', __FILE__)

Benchmark.ips do |x|
  string_array = $small_string_array.dup
  fixnum_array = $small_fixnum_array.dup
  time_array = $small_time_array.dup

  x.report "sort strings ascending" do |times|
    i = 0
    while i < times
      string_array.sort
      i += 1
    end
  end

  x.report "array sort! strings ascending" do |times|
    i = 0
    while i < times
      array2 = string_array.dup
      array2.sort!
      i += 1
    end
  end

  x.report "sort strings descending with block" do |times|
    i = 0
    while i < times
      string_array.sort { |a,b| b <=> a }
      i += 1
    end
  end

  x.report "sort! strings descending with block" do |times|
    i = 0
    while i < times
      array2 = string_array.dup
      array2.sort! { |a,b| b <=> a }
      i += 1
    end
  end

  x.report "sort numbers ascending" do |times|
    i = 0
    while i < times
      fixnum_array.sort
      i += 1
    end
  end

  x.report "array sort! numbers ascending" do |times|
    i = 0
    while i < times
      array2 = fixnum_array.dup
      array2.sort!
      i += 1
    end
  end

  x.report "sort numbers descending with block" do |times|
    i = 0
    while i < times
      fixnum_array.sort { |a,b| b <=> a }
      i += 1
    end
  end

  x.report "sort! numbers descending with block" do |times|
    i = 0
    while i < times
      array2 = fixnum_array.dup
      array2.sort! { |a,b| b <=> a }
      i += 1
    end
  end

  x.report "sort time ascending" do |times|
    i = 0
    while i < times
      time_array.sort
      i += 1
    end
  end

  x.report "array sort! time ascending" do |times|
    i = 0
    while i < times
      array2 = time_array.dup
      array2.sort!
      i += 1
    end
  end

  x.report "sort time descending with block" do |times|
    i = 0
    while i < times
      time_array.sort { |a,b| b <=> a }
      i += 1
    end
  end

  x.report "sort! time descending with block" do |times|
    i = 0
    while i < times
      array2 = time_array.dup
      array2.sort! { |a,b| b <=> a }
      i += 1
    end
  end

end
