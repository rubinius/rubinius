# encoding: utf-8

require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  string_mirror = Rubinius::Mirror.reflect "hellü 椎名深夏" * 1_000_000

  x.report "byte_to_character_index" do |times|
    i = 0
    while i < times
      string_mirror.byte_to_character_index(1_000_000)
      i += 1
    end
  end

end
