require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  empty_string = ""
  medium_string = "y" * 100
  long_string = "y" * 1_000
  non_matching_from_str = "z"
  matching_from_str = "y"
  to_str = "z"
  long_from_str = "a-y"
  long_non_matching_from_str = "a-x"
  long_to_str = "b-z"

  x.report "Empty string tr!" do |times|
    i = 0
    while i < times
      empty_string.dup.tr!(empty_string, empty_string)
      i += 1
    end
  end

  x.report "Medium string tr! with empty arguments" do |times|
    i = 0
    while i < times
      medium_string.dup.tr!(empty_string, empty_string)
      i += 1
    end
  end

  x.report "Medium string tr! with non match" do |times|
    i = 0
    while i < times
      medium_string.dup.tr!(non_matching_from_str, to_str)
      i += 1
    end
  end

  x.report "Medium string tr! with match" do |times|
    i = 0
    while i < times
      medium_string.dup.tr!(matching_from_str, to_str)
      i += 1
    end
  end

  x.report "Medium string tr! with a long from_str" do |times|
    i = 0
    while i < times
      medium_string.dup.tr!(long_from_str, long_to_str)
      i += 1
    end
  end

  x.report "Medium string tr! with a long non matching from_str" do |times|
    i = 0
    while i < times
      medium_string.dup.tr!(long_non_matching_from_str, empty_string)
      i += 1
    end
  end

  x.report "Long string tr! with empty arguments" do |times|
    i = 0
    while i < times
      long_string.dup.tr!(empty_string, empty_string)
      i += 1
    end
  end

  x.report "Long string tr! with non match" do |times|
    i = 0
    while i < times
      long_string.dup.tr!(non_matching_from_str, to_str)
      i += 1
    end
  end

  x.report "Long string tr! with match" do |times|
    i = 0
    while i < times
      long_string.dup.tr!(matching_from_str, to_str)
      i += 1
    end
  end

  x.report "Long string tr! with a long from_str" do |times|
    i = 0
    while i < times
      long_string.dup.tr!(long_from_str, long_to_str)
      i += 1
    end
  end

  x.report "Long string tr! with a long non matching from_str" do |times|
    i = 0
    while i < times
      long_string.dup.tr!(long_non_matching_from_str, empty_string)
      i += 1
    end
  end

  x.report "Empty string tr" do |times|
    i = 0
    while i < times
      empty_string.tr(empty_string, empty_string)
      i += 1
    end
  end
end
