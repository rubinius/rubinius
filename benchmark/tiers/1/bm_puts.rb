require 'benchmark'
require 'benchmark/ips'
require 'stringio'

# I want to test the performance of Kernel#puts but it's difficult
# to do so when we need to print out the results but *not* print
# out these test strings. I'm afraid that by using StringIO here
# I am not really testing the code paths that I want to.
#
# This $stdout override depends on the benchmarking library using
# and explicit STDOUT.puts/STDOUT.print

$stdout = StringIO.new
one_char = 'a'
string = "#{'a' * 1_000}"


Benchmark.ips do |x|

  x.report "puts on char" do |times|
    i = 0
    while i < times
      puts one_char
      i += 1
    end
  end

  x.report "puts string" do |times|
    i = 0
    while i < times
      puts string
      i += 1
    end
  end
end
