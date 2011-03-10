require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  int = "5"
  float = "5.0"
  empty = ""
  with_extra_text = "5.0 and some extra characters"

  x.report "#to_f with an integer in a string" do |times|
    i = 0
    while i < times
      int.to_f
      i += 1
    end
  end

  x.report "#to_f with a float in a string" do |times|
    i = 0
    while i < times
      float.to_f
      i += 1
    end
  end

  x.report "#to_f with an empty string" do |times|
    i = 0
    while i < times
      empty.to_f
      i += 1
    end
  end

  x.report "#to_f with a float and trailing text" do |times|
    i = 0
    while i < times
      with_extra_text.to_f
      i += 1
    end
  end
end
