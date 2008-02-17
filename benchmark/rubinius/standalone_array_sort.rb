require 'benchmark'

i = (ARGV.shift || 10).to_i
n = (ARGV.shift || 1_000).to_i

c = -(n / 2)
sorted    = Array.new(n).map! { c += 1 }

c = (n / 2)
reversed  = Array.new(n).map! { c -= 1 }

same  = Array.new(n).map! { 10 }
two  = Array.new(n).map! { rand 2 }
three = Array.new(n).map! { rand 3 }

random = Array.new(n).map { num = rand(n); rand(2) % 2 == 0 ? num : -num }

str = 'Hello there how goes?'
strings = Array.new(n).map! {
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}" \
  "#{(65 + rand(58)).chr}"[0..(rand(15 + 6))]
}

puts "Numeric, #{n} of each, #{i} loops:"
puts "  - Sorted:   [1, 2, 3, 4, 5]"
puts "  - Reversed: [5, 4, 3, 2, 1]"
puts "  - Same:     [1, 1, 1, 1, 1]"
puts "  - Two:      [1, 2, 1, 1, 2]"
puts "  - Three:    [3, 2, 3, 1, 2]"
puts "  - Random:   [3, 5, 1, 4, 2]"
puts
puts "Strings:"
puts "  - #{strings.size} strings, each with 6-21 characters\n\n"
puts
puts "-- #sort --"

Benchmark.bmbm {|bench|
  bench.report("sorted")          { i.times { sorted.sort } }
  bench.report("sorted block")    { i.times { sorted.sort {|a, b| a <=> b} } }
  bench.report("reversed")        { i.times { reversed.sort } }
  bench.report("reversed block")  { i.times { reversed.sort {|a, b| a <=> b} } }
  bench.report("same")            { i.times { same.sort } }
  bench.report("same block")      { i.times { same.sort {|a, b| a <=> b} } }
  bench.report("two")             { i.times { two.sort } }
  bench.report("two block")       { i.times { two.sort {|a, b| a <=> b} } }
  bench.report("three")           { i.times { three.sort } }
  bench.report("three block")     { i.times { three.sort {|a, b| a <=> b} } }
  bench.report("random")          { i.times { random.sort } }
  bench.report("random block")    { i.times { random.sort {|a, b| a <=> b} } }
  bench.report("strings ")        { i.times { strings.sort } }
  bench.report("strings block")   { i.times { strings.sort {|a, b| a <=> b} } }
}

puts
puts "-- #sort! --"

Benchmark.bmbm {|bench|
  sorted2, sorted3 = sorted.dup, sorted.dup
  reversed2, reversed3 = reversed.dup, reversed.dup
  same2, same3 = same.dup, same.dup
  two2, two3 = two.dup, two.dup
  three2, three3 = three.dup, three.dup
  random2, random3 = random.dup, random.dup
  strings2, strings3 = strings.dup, strings.dup

  bench.report("sorted")          { i.times { sorted2.sort } }
  bench.report("sorted block")    { i.times { sorted3.sort {|a, b| a <=> b} } }
  bench.report("reversed")        { i.times { reversed2.sort } }
  bench.report("reversed block")  { i.times { reversed3.sort {|a, b| a <=> b} } }
  bench.report("same")            { i.times { same2.sort } }
  bench.report("same block")      { i.times { same3.sort {|a, b| a <=> b} } }
  bench.report("two")             { i.times { two2.sort } }
  bench.report("two block")       { i.times { two3.sort {|a, b| a <=> b} } }
  bench.report("three")           { i.times { three2.sort } }
  bench.report("three block")     { i.times { three3.sort {|a, b| a <=> b} } }
  bench.report("random")          { i.times { random2.sort } }
  bench.report("random block")    { i.times { random3.sort {|a, b| a <=> b} } }
  bench.report("strings ")        { i.times { strings2.sort } }
  bench.report("strings block")   { i.times { strings3.sort {|a, b| a <=> b} } }
}
