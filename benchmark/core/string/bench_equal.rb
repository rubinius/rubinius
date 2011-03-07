require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  string_a = "#{'a' * 1_000}"
  first_char_different = "b#{'a' * 1_000}"
  last_char_different = "#{'a' * 1_000}b"
  same = string_a.dup

  x.report "string ==, match" do |times|
    i = 0
    while i < times
      string_a == same
      i += 1
    end
  end

  x.report "string ==, mismatch first char" do |times|
    i = 0
    while i < times
      string_a == first_char_different
      i += 1
    end
  end

  x.report "string ==, mismatch last char" do |times|
    i = 0
    while i < times
      string_a == last_char_different
      i += 1
    end
  end
end
