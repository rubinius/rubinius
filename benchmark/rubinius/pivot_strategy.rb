require 'benchmark'

puts "Pivot strategy"

$VERBOSE = nil

$stages     = (ARGV.shift || 3).to_i
$thresholds = []

unless ARGV.empty?
  $thresholds << ['Supplied', ARGV.map {|i| i.to_i }]
else
  $thresholds <<  ['Spread', [5, 10, 25, 50, 100, 125, 175, 250, 500, 750, 1000, 1500]]
  $thresholds <<  ['Close', [55, 75, 85, 95, 105, 125, 495, 500, 505, 525]]
end


#print "Building strings..."
#strings   = []
#strings  << ('Hello there how aaaa'..'Hello there how iiii').to_a
#strings  << ('Hello there how aaaa'..'Hello there how ffff').to_a
#strings  << ('Hello there how aaaa'..'Hello there how cccc').to_a
#strings  << ('Hello there how aaaa'..'Hello there how bbbb').to_a
#strings  << ('Hello there how aaaa'..'Hello there how asss').to_a
#strings  << ('Hello there how aaaa'..'Hello there how ammm').to_a
#strings  << ('Hello there how aaaa'..'Hello there how aiii').to_a
#strings  << ('Hello there how aaaa'..'Hello there how ahff').to_a
#strings  << ('Hello there how aaaa'..'Hello there how afff').to_a
#strings  << ('Hello there how aaaa'..'Hello there how abbb').to_a
#strings  << ('Hello there how aaaa'..'Hello there how aaii').to_a
#strings  << ('Hello there how aaaa'..'Hello there how aaff').to_a
#strings  << ('Hello there how aaaa'..'Hello there how aacc').to_a
#strings  << ('Hello there how aaaa'..'Hello there how aabb').to_a
#strings  << ('Hello there how aaaa'..'Hello there how aaam').to_a
#strings  << ('Hello there how aaaa'..'Hello there how aaag').to_a
#strings  << ('Hello there how aaaa'..'Hello there how aaae').to_a
#
#print "mixing strings..."
#mixed = strings.map {|set|
#  (set + set.dup).sort_by { rand }
#}
#puts "strings set up"

#puts "\nRunning strings\n"
#Benchmark.bmbm do |bench|
#  [10, 50, 100, 500, 1000, 5000, 25000, 75000, 200000].each do |median|
#    mixed.reverse_each {|set|
#      Array.const_set :MEDIAN_THRESHOLD, median
#      bench.report("#{set.size} mixed strs at #{median} med") { set.sort }
#    end
#  end
#
#  [10, 50, 100, 500, 1000, 5000, 25000, 75000, 200000].each do |median|
#    strings.reverse_each do |set|
#      Array.const_set :MEDIAN_THRESHOLD, median
#      bench.report("#{set.size} straight strs at #{median} med") { set.sort }
#    end
#  end
#end

def make_numbers(*sizes)
  $stderr.puts "  Building numeric arrays of sizes #{sizes.inspect}..."

  a = sizes.inject([]) {|memo, i|
        n = i * 2
        memo << ((-n...i).to_a + (-i...n).to_a).sort_by { rand }.reject { rand % 5 == 0 }
      }

  $stderr.puts "  Arrays built..."
  a
end

def pivot_run(sets)
  $stderr.puts "  Thresholds: #{$thresholds.inspect}..."
  $stderr.puts "  Running benchmark..."

  sets.each do |set|
    $thresholds.each do |message, numbers|
      puts message

      Benchmark.bmbm do |bench|
        numbers.each do |threshold|
          Array.const_set :MEDIAN_THRESHOLD, threshold
          bench.report("#{set.size} numbers at #{threshold} median threshold") { set.sort }
        end
      end
      $stderr.puts
    end
  end

  $stderr.puts "Done.\n\n"
end

def stage(number, message)
  abort "All requested stages run" if $stages < number 
  $stderr.puts "Stage #{number}: #{message}"

  yield
end


# We have three 'stages'

stage 1, "small Arrays" do
  pivot_run(make_numbers(1, 3, 7, 10, 25, 50, 66, 111, 250, 500))
end

stage 2, "medium Arrays" do
  pivot_run(make_numbers(750, 1000, 2500, 5000, 7500, 10_000))
end

stage 3, "huge Arrays" do
  pivot_run(make_numbers(25_000, 50_000, 100_000, 250_000))
end
