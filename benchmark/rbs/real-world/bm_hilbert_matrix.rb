# Submitted by M. Edward (Ed) Borasky
require 'hilbert' # also brings in mathn, matrix, rational and complex

def run_hilbert(dimension)
  m = hilbert(dimension)
  print "Hilbert matrix of dimension #{dimension} times its inverse = identity? "
  k = m * m.inv
  print "#{k==Matrix.I(dimension)}\n"
  m = nil # for the garbage collector
  k = nil
end

Bench.run [10, 20, 30, 40, 50, 60] do |n|
  run_hilbert(n)
end
