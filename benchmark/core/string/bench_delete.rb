require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  empty_string = ""
  long_string = "y" * 1_000
  huge_string = "y" * 1_000_000
  non_matching_other = "z"
  matching_other = "y"

  x.report "Long string delete! with empty argument" do |times|
    i = 0
    while i < times
      long_string.dup.delete!(empty_string)
      i += 1
    end
  end

  x.report "Long string delete! with non match" do |times|
    i = 0
    while i < times
      long_string.dup.delete!(non_matching_other)
      i += 1
    end
  end

  x.report "Long string delete! with match" do |times|
    i = 0
    while i < times
      long_string.dup.delete!(matching_other)
      i += 1
    end
  end

  x.report "Huge string delete! with empty argument" do |times|
    i = 0
    while i < times
      huge_string.dup.delete!(empty_string)
      i += 1
    end
  end

  x.report "Huge string delete! with non match" do |times|
    i = 0
    while i < times
      huge_string.dup.delete!(non_matching_other)
      i += 1
    end
  end

  x.report "Huge string delete! with match" do |times|
    i = 0
    while i < times
      huge_string.dup.delete!(matching_other)
      i += 1
    end
  end

  x.report "Empty string delete!" do |times|
    i = 0
    while i < times
      empty_string.dup.delete!(empty_string)
      i += 1
    end
  end

  x.report "Long string delete with empty argument" do |times|
    i = 0
    while i < times
      long_string.delete(empty_string)
      i += 1
    end
  end

  x.report "Long string delete with non match" do |times|
    i = 0
    while i < times
      long_string.delete(non_matching_other)
      i += 1
    end
  end

  x.report "Long string delete with match" do |times|
    i = 0
    while i < times
      long_string.delete(matching_other)
      i += 1
    end
  end

  x.report "Huge string delete with empty argument" do |times|
    i = 0
    while i < times
      huge_string.delete(empty_string)
      i += 1
    end
  end

  x.report "Huge string delete with non match" do |times|
    i = 0
    while i < times
      huge_string.delete(non_matching_other)
      i += 1
    end
  end

  x.report "Huge string delete with match" do |times|
    i = 0
    while i < times
      huge_string.delete(matching_other)
      i += 1
    end
  end

  x.report "Empty string delete" do |times|
    i = 0
    while i < times
      empty_string.delete(empty_string)
      i += 1
    end
  end

end
