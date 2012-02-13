require 'benchmark'
require 'benchmark/ips'
require File.expand_path('../shared_array.rb', __FILE__)

# Should be expanded to include tests that pass ranges and start,length to
# the #[] method so that we have complete coverage of all potential uses
# for these method calls

Benchmark.ips do |x|
  string_array = $small_string_array.dup
  fixnum_array = $small_fixnum_array.dup

  x.report "#at(0) with string elements" do |times|
    i = 0
    while i < times
      string_array.at(0)
      i += 1
    end
  end

  x.report "#at(size / 2) with string elements" do |times|
    middle = string_array.size / 2
    i = 0
    while i < times
      string_array.at(middle)
      i += 1
    end
  end

  x.report "#at(-1) with string elements" do |times|
    i = 0
    while i < times
      string_array.at(-1)
      i += 1
    end
  end

  x.report "#at(0) with fixnum elements" do |times|
    i = 0
    while i < times
      fixnum_array.at(0)
      i += 1
    end
  end

  x.report "#at(size / 2) with fixnum elements" do |times|
    middle = fixnum_array.size / 2
    i = 0
    while i < times
      fixnum_array.at(middle)
      i += 1
    end
  end

  x.report "#at(-1) with fixnum elements" do |times|
    i = 0
    while i < times
      fixnum_array.at(-1)
      i += 1
    end
  end

  x.report "#[0] with string elements" do |times|
    i = 0
    while i < times
      string_array[0]
      i += 1
    end
  end

  x.report "#[size / 2] with string elements" do |times|
    middle = string_array.size / 2
    i = 0
    while i < times
      string_array[middle]
      i += 1
    end
  end

  x.report "#[-1] with string elements" do |times|
    i = 0
    while i < times
      string_array[-1]
      i += 1
    end
  end

  x.report "#[0] with fixnum elements" do |times|
    i = 0
    while i < times
      fixnum_array[0]
      i += 1
    end
  end

  x.report "#[size / 2] with fixnum elements" do |times|
    middle = fixnum_array.size / 2
    i = 0
    while i < times
      fixnum_array[middle]
      i += 1
    end
  end

  x.report "#[-1] with fixnum elements" do |times|
    i = 0
    while i < times
      fixnum_array[-1]
      i += 1
    end
  end

end
