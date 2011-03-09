require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  int = "5"
  float = "5.0"
  empty = ""
  with_extra_text = "5 and some extra characters"

  x.report "#to_i with an integer in a string" do |times|
    i = 0
    while i < times
      int.to_i
      i += 1
    end
  end

  x.report "#to_i with a float in a string" do |times|
    i = 0
    while i < times
      float.to_i
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

  x.report "#to_i with an integer and extra text" do |times|
    i = 0
    while i < times
      with_extra_text.to_i
      i += 1
    end
  end
end
