require 'benchmark'
require 'benchmark/ips'
require 'tempfile'

file = Tempfile.new('puts_benchmark')
one_char = 'a'
string = "#{'a' * 1_000}"

begin

  Benchmark.ips do |x|

    x.report "puts one char" do |times|
      i = 0
      while i < times
        file.puts one_char
        i += 1
      end
    end

    x.report "puts string" do |times|
      i = 0
      while i < times
        file.puts string
        i += 1
      end
    end
  end

ensure
  file.close
  file.unlink
end
