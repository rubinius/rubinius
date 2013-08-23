require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  small_int = "5"
  large_int = "14213542524"
  small_float = "5.0"
  large_float = "5415125422.0"
  empty = ""
  small_with_extra_text = "5 and some extra characters"
  large_with_extra_text = "311334315415 and some extra characters"

  x.report "#to_i with a small integer in a string" do |times|
    i = 0
    while i < times
      small_int.to_i
      i += 1
    end
  end

  x.report "#to_i with a large integer in a string" do |times|
    i = 0
    while i < times
      large_int.to_i
      i += 1
    end
  end

  x.report "#to_i with a small float in a string" do |times|
    i = 0
    while i < times
      small_float.to_i
      i += 1
    end
  end

  x.report "#to_i with a large float in a string" do |times|
    i = 0
    while i < times
      large_float.to_i
      i += 1
    end
  end

  x.report "#to_i with an empty string" do |times|
    i = 0
    while i < times
      empty.to_i
      i += 1
    end
  end

  x.report "#to_i with a small integer and extra text" do |times|
    i = 0
    while i < times
      small_with_extra_text.to_i
      i += 1
    end
  end

  x.report "#to_i with a large integer and extra text" do |times|
    i = 0
    while i < times
      large_with_extra_text.to_i
      i += 1
    end
  end

end
