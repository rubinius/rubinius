require 'benchmark'

#puts '------------'
#p [3, -10, 5, 1000, -34, 9, 3, 0, -1, -1].sort
#exit

i = (ARGV.shift || 10).to_i
n = (ARGV.shift || 1_000).to_i

c = -(n / 2)
sorted    = Array.new(n).map! { c += 1 }

c = (n / 2)
reversed  = Array.new(n).map! { c -= 1 }

random    = sorted.sort_by { rand }
all_same  = Array.new(n).map! { 10 }
two_vary  = Array.new(n).map! { rand 2 }
three_set = Array.new(n).map! { rand 3 }

mixed     = random.map {|o| rand(4) % 3 ? random.at(rand(random.size)) : o }
strings   = ('Hello there how aaaa'..'Hello there how iiii').to_a[0...1462]

puts "Numeric, #{n} of each, #{i} loops:"
puts "  - Sorted:   [1, 2, 3, 4, 5]"
puts "  - Reversed: [5, 4, 3, 2, 1]"
puts "  - Random:   [3, 5, 1, 4, 2]"
puts "  - Same:     [1, 1, 1, 1, 1]"
puts "  - Two:      [1, 2, 1, 1, 2]"
puts "  - Three:    [3, 2, 3, 1, 2]"
puts "  - Mixed:    [2, 2, 7, -1, 0]"
puts
puts "Strings:"
puts "  - #{strings.size} strings, each with #{strings.first.length} characters\n\n"
puts
puts "-- #sort --"

Benchmark.bmbm {|bench|
  bench.report("sorted  ")        { i.times { sorted.sort } }
  bench.report("sorted block")    { i.times { sorted.sort {|a, b| a <=> b} } }
  bench.report("reversed")        { i.times { reversed.sort } }
  bench.report("reversed block")  { i.times { reversed.sort {|a, b| a <=> b} } }
  bench.report("random  ")        { i.times { random.sort } }
  bench.report("random block")    { i.times { random.sort {|a, b| a <=> b} } }
  bench.report("same    ")        { i.times { all_same.sort } }
  bench.report("random block")    { i.times { random.sort {|a, b| a <=> b} } }
  bench.report("two     ")        { i.times { two_vary.sort } }
  bench.report("random block")    { i.times { random.sort {|a, b| a <=> b} } }
  bench.report("three   ")        { i.times { three_set.sort } }
  bench.report("random block")    { i.times { random.sort {|a, b| a <=> b} } }
  bench.report("mixed   ")        { i.times { mixed.sort } }
  bench.report("mixed block")     { i.times { mixed.sort {|a, b| a <=> b} } }

  bench.report("strings ")        { strings.sort }
  bench.report("strings block")   { strings.sort {|a, b| a <=> b} }
}

puts
puts "-- #sort! --"

Benchmark.bmbm {|bench|
  bench.report("sorted  ")        { i.times { sorted.sort! } }
  bench.report("sorted block")    { i.times { sorted.sort! {|a, b| a <=> b} } }
  bench.report("reversed")        { i.times { reversed.sort! } }
  bench.report("reversed block")  { i.times { reversed.sort! {|a, b| a <=> b} } }
  bench.report("random  ")        { i.times { random.sort! } }
  bench.report("random block")    { i.times { random.sort! {|a, b| a <=> b} } }
  bench.report("same    ")        { i.times { all_same.sort! } }
  bench.report("random block")    { i.times { random.sort! {|a, b| a <=> b} } }
#  p two_vary
#  bench.report("two     ")        { i.times { two_vary.sort! } }
  bench.report("random block")    { i.times { random.sort! {|a, b| a <=> b} } }
  bench.report("three   ")        { i.times { three_set.sort! } }
  bench.report("random block")    { i.times { random.sort! {|a, b| a <=> b} } }
  bench.report("mixed   ")        { i.times { mixed.sort! } }
  bench.report("mixed block")     { i.times { mixed.sort! {|a, b| a <=> b} } }

  bench.report("strings ")        { strings.sort! }
  bench.report("strings block")   { strings.sort! {|a, b| a <=> b} }
}
