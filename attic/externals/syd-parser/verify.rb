require 'sydparse'

GC.disable

file = ARGV.shift
puts "Loading #{file}..."

syd = SydneyParser.load_file File.open(file)

puts "Converting to sexp..."
sx = syd.sexp

ac = Accelload.new(sx)

puts "Comparing sexps..."
s2 = ac.to_pt

sx1 = syd.sexp
sx2 = s2.sexp

require 'pp'

if sx1 == sx2
  puts "Same!"
else
  puts "EEK! Different!"
  f1 = "f1.sexp"
  f2 = "f2.sexp"
  puts "saving sexps..."
  File.open(f1, "w") { |f| f << sx1.inspect.gsub("]", "]\n") }
  puts "now for 2nd.."
  File.open(f2, "w") { |f| f << sx2.inspect.gsub("]", "]\n") }
end