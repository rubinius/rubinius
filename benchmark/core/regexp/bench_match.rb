# encoding: utf-8

require 'benchmark'
require 'benchmark/ips'

strings = ["zombies".force_encoding("US-ASCII"),
           "zümbies".force_encoding("UTF-8"),
           "z\x00mbies".force_encoding("ASCII-8BIT")]

# Benchmark for showing issues when encodings of strings change.
# This behavior is present in Rails where it happens often for example
# for escaping html safe buffers.

Benchmark.ips do |x|
  x.report "Regexp#match on same strings" do |times|
    i = 0
    while i < times
      /a-z/.match "zombies"
      i += 1
    end
  end

  x.report "Regexp#match on changing string encodings" do |times|
    i = 0
    while i < times
      strings.each do |str|
        /a-z/.match str
      end
      i += 1
    end
  end

end

