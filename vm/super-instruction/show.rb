
def show(data)
  out = data.sort_by { |key, val| val }

  out.reverse[0,50].each do |combo, count|
    puts "#{combo.inspect}: #{count}"
  end
end

class Array
  def included_in?(bigger)
    return false if bigger.size < size

    idx = 0
    while self[idx] != bigger[idx]
      idx += 1
      return false if idx >= size
    end

    idx.upto(size - 1) do |i|
      return false if self[i] != bigger[i]
    end

    return true
  end

  def ends_with?(other)
    start = size - other.size
    self[start..-1] == other
  end

  def begins_with?(other)
    self[0,other.size] == other
  end
end

def prune_subsets(data)
  sizes = data.keys.sort.reverse
  until sizes.empty?
    size = sizes.shift
    data[size].each do |combo, count|
      sizes.each do |smaller|
        data[smaller].each do |entry|
          if entry[0].included_in?(combo)
            entry[1] -= count
          end
        end
      end
    end
  end
end

class Sequence
  def initialize(combo, count)
    @combo, @count = combo, count
  end

  attr_reader :combo, :count
end

class Weight
  def initialize(weight)
    @weight = weight
    @entries = []
  end

  def <<(combo)
    @entries << combo
  end

  attr_reader :entries, :weight
end

weighted = []

data = Marshal.load File.read(ARGV.shift)

all = Hash.new { |h,k| h[k] = [] }
data.each do |seq, count|
  all[count] << seq
end

def remove_subs(seqs)
  to_remove = []
  seqs.each do |seq|
    delete = false
    seqs.each do |other|
      if seq != other
        if other.ends_with?(seq) or other.begins_with?(seq)
          to_remove << seq
          break
        end
      end
    end
  end

  seqs - to_remove
end

data = []
all.keys.sort.reverse[0,100].each do |count|
  seqs = all[count].sort { |a,b| b.size <=> a.size }

  data << seqs.first
end

=begin

all = Hash.new { |h,k| h[k] = [] }

data.keys.sort.each do |size|
  data[size].each do |combo, count|
    all[count] << combo

    index = count * (size - 1)
    if sub = weighted[index]
      sub << Sequence.new(combo, count)
    else
      weight = Weight.new(index)
      weight << Sequence.new(combo, count)
      weighted[index] = weight
    end
  end
end

all.keys.sort.reverse[0,20].each do |count|
  puts "#{count}:"
  p all[count]
end

data = []
shown = 0
weighted.reverse.each do |weight|
  next unless weight
  break if shown == 100
  puts "#{weight.weight}:"

  shown += 1
  weight.entries.each do |sequence|
    data << sequence.combo
    puts "  #{sequence.count}: #{sequence.combo.inspect}"
  end
end

=end

if output = ARGV.shift
  require 'yaml'

  File.open(output, "w") do |f|
    f.puts data.to_yaml
  end
end
