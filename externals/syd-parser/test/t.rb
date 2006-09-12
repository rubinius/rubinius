
require 'sydparse'
require 'find'
require 'pp'

STDOUT.sync = true

def test_file(path)
    pid = fork {
        print "#{path}: "
        io = File.open(path)
        print "o"
        syd = SydneyParser.load_file io
        print "p"
        sx = syd.sexp
        # sx = []
        # pp sx
        print "x"
        sx.inspect
        puts " ok"
        io.close
        }
        Process.wait(pid)
end

path = ARGV.shift

Find.find(path) do |path|
    next if File.directory?(path)
    if /\.rb$/.match(path)
        test_file(path)
    end
end
exit
io = File.open(ARGV.shift)

syd = SydneyParser.load_file io

p syd

sx = syd.sexp
p sx.size
ary = sx[0]
a3 = ary[2]
# p a3.size
# p a3[1][1][2]

pp sx
exit
File.open("last.o", "w") do |f|
    f << Marshal.dump(sx)
end
# pp syd.sexp
