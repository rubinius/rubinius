require 'benchmark'

$VERBOSE = nil

i = (ARGV.shift || 10).to_i
n = (ARGV.shift || 1_000).to_i / 2

print "Building strings..."
strings   = []
strings  << ('Hello there how aaaa'..'Hello there how iiii').to_a
strings  << ('Hello there how aaaa'..'Hello there how ffff').to_a
strings  << ('Hello there how aaaa'..'Hello there how cccc').to_a
strings  << ('Hello there how aaaa'..'Hello there how bbbb').to_a
strings  << ('Hello there how aaaa'..'Hello there how asss').to_a
strings  << ('Hello there how aaaa'..'Hello there how ammm').to_a
strings  << ('Hello there how aaaa'..'Hello there how aiii').to_a
strings  << ('Hello there how aaaa'..'Hello there how ahff').to_a
strings  << ('Hello there how aaaa'..'Hello there how afff').to_a
strings  << ('Hello there how aaaa'..'Hello there how abbb').to_a
strings  << ('Hello there how aaaa'..'Hello there how aaii').to_a
strings  << ('Hello there how aaaa'..'Hello there how aaff').to_a
strings  << ('Hello there how aaaa'..'Hello there how aacc').to_a
strings  << ('Hello there how aaaa'..'Hello there how aabb').to_a
strings  << ('Hello there how aaaa'..'Hello there how aaam').to_a
strings  << ('Hello there how aaaa'..'Hello there how aaag').to_a
strings  << ('Hello there how aaaa'..'Hello there how aaae').to_a

print "mixing strings..."
mixed = strings.map {|set|
  (set + set.dup).sort_by { rand }
}
puts "strings set up"

print "Building numbers..."
numbers  = []
[5, 7, 15, 50, 100, 250, 500, 1000, 5000, 10000, 25000].each {|i|
  n = i * 2
  numbers << ((-n...i).to_a + (-i...n).to_a).sort_by { rand }.reject { rand % 5 == 0 }
}
puts "numbers set up"

puts "\nRunning strings\n"
Benchmark.bmbm do |bench|
  mixed.reverse_each {|set|
    [10, 50, 100, 500, 1000, 5000, 25000, 75000, 200000].each do |median|
      Array.const_set :MEDIAN_THRESHOLD, median
      bench.report("#{set.size} mixed strs at #{median} med") { set.sort }
    end
  }
  strings.reverse_each {|set|
    [10, 50, 100, 500, 1000, 5000, 25000, 75000, 200000].each do |median|
      Array.const_set :MEDIAN_THRESHOLD, median
      bench.report("#{set.size} straight strs at #{median} med") { set.sort }
    end
  }
end

puts "\nRunning numbers\n"
Benchmark.bmbm do |bench|
  numbers.reverse_each {|set|
    [10, 50, 100, 500, 1000, 5000, 25000, 75000, 200000].each do |median|
      Array.const_set :MEDIAN_THRESHOLD, median
      bench.report("#{set.size} numbers at #{median} med") { set.sort }
    end
  }
end
