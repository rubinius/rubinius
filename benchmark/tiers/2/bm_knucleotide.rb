# The Computer Language Shootout
# http://shootout.alioth.debian.org
#
# contributed by jose fco. gonzalez
# modified by Sokolov Yura
# Adapted for the Ruby Benchmark Suite.

def frequency(seq, length)
  n = seq.length - length + 1
  table = Hash.new(0)
  f = nil
  i = nil

  0.upto(length-1) do |f|
    (f...n).step(length) do |i|
      table[seq[i,length]] += 1
    end
  end
  [n,table]
end

def sort_by_freq(seq, length)
  n, table = frequency(seq, length)
  a, b, v = nil, nil, nil
  table.sort{|a,b| b[1] <=> a[1]}.each do |v|
      puts "%s %.3f" % [v[0].upcase,((v[1]*100).to_f/n)]
  end
    puts
end

def find_seq(seq, s)
  n, table = frequency(seq, s.length)
  puts "#{table[s].to_s}\t#{s.upcase}"
end

def Bench.run
  seq = String.new
  fname = File.dirname(__FILE__) + "/fasta.input"
  File.open(fname, "r").each_line do |line|
    seq << line.chomp
  end
  [1,2].each {|i| sort_by_freq(seq, i) }
  %w(ggt ggta ggtatt ggtattttaatt ggtattttaatttatagt).each{|s| find_seq(seq, s) }
end
