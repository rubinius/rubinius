require 'benchmark'

i = (ARGV.shift || 10).to_i
n = (ARGV.shift || 1_000).to_i / 2

sorted    = (-n...n).to_a
reversed  = (-n...n).to_a.reverse
random    = (-n...n).to_a.sort_by { rand }
mixed     = (1..1).to_a
strings   = ('Hello there how aaaa'..'Hello there how iiii').to_a

puts "#{sorted.size} sorted, #{reversed.size} reverse-sorted, #{random.size} randomized\n"
puts "#{strings.size} #{strings.first.size}-character strings\n\n"

Benchmark.bmbm {|bench|
  bench.report('sorted')          { i.times { sorted.sort } }
  bench.report('sorted block')    { i.times { sorted.sort {|a, b| b <=> a} } }
  bench.report('reversed')        { i.times { reversed.sort } }
  bench.report('reversed block')  { i.times { reversed.sort  {|a, b| b <=> a} } }
  bench.report('random')          { i.times { random.sort } }
  bench.report('random block')    { i.times { random.sort  {|a, b| b <=> a} } }
  bench.report('strings')         { strings.sort }
  bench.report('strings block')   { strings.sort {|a, b| b <=> a} }
}
