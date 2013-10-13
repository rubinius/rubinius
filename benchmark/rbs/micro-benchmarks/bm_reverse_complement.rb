# The Great Computer Language Shootout
# http://shootout.alioth.debian.org/
#
# Contributed by Peter Bjarke Olsen
# Modified by Doug King
# Adapted for the Ruby Benchmark Suite.

def revcomp(seq)
  seq.reverse!.tr!('wsatugcyrkmbdhvnATUGCYRKMBDHVN','WSTAACGRYMKVHDBNTAACGRYMKVHDBN')
  stringlen = seq.length
  0.step(stringlen-1,60) {|x| puts seq.slice(x,60) }
end

Bench.run [1] do |n|
  seq = Array.new

  fname = File.dirname(__FILE__) + "/fasta.input" 
  File.open(fname, "r").each_line do |line|
    seq << line.chomp
  end

  1000.times do
   revcomp(seq.join)
  end
end
