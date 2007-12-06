# also brings in mathn, matrix, rational and complex
require File.expand_path(File.join(File.dirname(__FILE__),'hilbert'))

require 'benchmark'

# define the method to run a benchmark
def do_benchmark(dimension) # actually run one benchmark
  puts Benchmark.measure {
    m = hilbert(dimension) # create a matrix
    print "Hilbert matrix of dimension #{dimension} times its inverse = identity? "
    k = m*m.inv
    print "#{k==Matrix.I(dimension)}\n"
    m = nil # for the garbage collector
    k = nil
  }
end

# main program
dimension = 1 # default is a scalar
dimension = ARGV[0].to_i if ARGV.length > 0

if dimension>1 # just run once with given dimension
  do_benchmark(dimension)

else # loop until user is tired
  tired = false # we aren't tired yet :)
  until tired
    do_benchmark(dimension)
    puts "\nThe next test will run about 8 times (or more) as long as the last one."
    print "Just \"Enter\" to continue, or enter a non-empty string to quit:"
    tired = gets !~ /^$/
    dimension *= 2
  end
end
