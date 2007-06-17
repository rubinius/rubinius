
require 'sydparse'
require 'pp'

STDOUT.sync = true

path = ARGV.shift
io = File.open(path)
if ARGV.shift
    syd = SydneyParser.load_file io
    pp syd.sexp
    exit
end
pp SydneyParser.unified_sexp(io)
exit
syd = SydneyParser.load_file io, false, true
sx = syd.sexp(true)
pp sx
com = syd.comments
p syd.collapse_per_line(com)
