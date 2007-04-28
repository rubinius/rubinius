# &, *, +, -, <<, <=>, ==, [], []=, abbrev, assoc, at, average,
# clear, collect, collect!, compact, compact!, concat, dclone,
# delete, delete_at, delete_if, each, each_index, empty?, eql?,
# fetch, fill, first, flatten, flatten!, frozen?, hash, include?,
# index, indexes, indices, initialize_copy, insert, inspect, join,
# last, length, map, map!, nitems, pack, pop, pretty_print,
# pretty_print_cycle, push, quote, rassoc, reject, reject!, replace,
# reverse, reverse!, reverse_each, rindex, select, shift, size,
# slice, slice!, sort, sort!, to_a, to_ary, to_s, to_yaml, transpose,
# uniq, uniq!, unshift, values_at, yaml_initialize, zip, |

class Array
  def __ivars__; nil; end

  def to_s
    "#<Array:0x#{object_id.to_s(16)} #{@total} elements>"
  end
  
  def inspect
    "[#{join(", ", :inspect)}]"
  end
  
  def each
    i = 0
    while i < @total
      yield @tuple.at(i)
      i += 1
    end
    self
  end
  
  def reverse_each
    i = @total
    while i > 0
      i -= 1
      yield @tuple.at(i)
    end
    self
  end
  
  def ==(other)
    return false unless other.kind_of?(Array)
    return false if @total != other.size
    each_with_index do |o, i|
      return false unless o == other[i]
    end
    return true
  end
  
  def map!
    i = 0
    each do |a|
      self[i] = yield(a)
      i += 1
    end
    return self
  end
  
  def join(sep='', meth=:to_s)
    str = ""
    return str if @total == 0
    tuple.join_upto(sep, @total, meth)
  end
  
  def <<(ent)
    self[@total] = ent
    self
  end
  
  def push(*args)
    args.each do |ent|
      self[@total] = ent
    end
    self
  end
  
  def compact
    out = []
    each do |ent|
      out << ent unless ent.nil?
    end
    return out
  end
  
  def compact!
    replace(compact)
  end

  # THIS MUST NOT BE REMOVED. the kernel requires a simple
  # Array#[] to work while parts of the kernel boot.
  def [](idx)    
    if idx >= @total
      return nil
    end

    @tuple.at(idx)
  end

  def first
    return nil if @total == 0
    @tuple.at(0)
  end
  
  def last(n=1)
    return nil if @total == 0
    return @tuple.at(@total-1) if n == 1
    ary = []
    for i in (@total-n)..(@total-1)
      ary.push @tuple.at(i)
    end
    return ary
  end
  
  def size
    @total
  end

  def length
    @total
  end
  
  def empty?
    @total == 0
  end
    
  def []=(idx, ent)
    if idx >= @tuple.fields
      nt = Tuple.new(idx + 10)
      nt.copy_from @tuple, 0
      @tuple = nt
    end

    @tuple.put idx, ent
    if idx >= @total - 1
      @total = idx + 1
    end
    return ent
  end
  
  def unshift(val)
    @tuple = @tuple.shifted(1)
    
    @tuple.put 0, val
    @total += 1
    return self
  end
  
  def +(other)
    out = dup()
    other.each { |e| out << e }
    return out
  end
  
  def <=>(other)
    each_with_index do |a, i| #Should use zip, but it wasn't liking it.
      return -1 if a.nil?
      return 1 if other[i].nil?
      dif = a <=> other[i]
      return dif unless dif == 0
    end
    return 0 #If no elements were different, then the arrays are identical.
  end
  
  def replace(other)
    @tuple = other.tuple.dup
    @total = other.total
    return self
  end
  
  def clear
    @tuple = Tuple.new(0)
    @total = 0
    return self
  end
  
  def dup
    ary = []
    each { |e| ary << e }
    return ary
  end
  
  def shift
    return nil if empty?
    
    ele = self[0]
    
    @tuple = @tuple.shift
    @total -= 1
    return ele
  end
  
  def pop
    return nil if empty?
    
    # TODO if total is a lot less than size of the tuple, 
    # the tuple should be resized down.
    
    ele = last()
    @total -= 1
    return ele
  end
end
