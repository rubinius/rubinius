class Array
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
  
  def index(val)
    i = 0
    while i < @total
      return i if val == @tuple.at(i)
      i += 1
    end
    return nil
  end
  
  def ==(other)
    return false unless other.kind_of?(Array)
    return false if @total != other.size
    i = 0
    while i < @total
      return false unless self[i] == other[i]
      i += 1
    end
    return true
  end
  
  def map
    out = []
    each do |a|
      out << yield(a)
    end
    return out
  end
  
  def map!
    i = 0
    each do |a|
      self[i] = yield(a)
      i += 1
    end
    return self
  end
  
  def join(sep, meth=:to_s)
    str = ""
    return str if @total == 0
    tuple.join_upto(sep, @total, meth)
  end
  
  def <<(ent)
    self[@total] = ent
  end
  
  def push(ent)
    self[@total] = ent
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
    
  def [](idx)    
    if idx >= @total
      return nil
    end
    
    @tuple.at(idx)
  end
  
  def first
    @tuple.at(0)
  end
  
  def last
    return nil if @total == 0
    @tuple.at(@total-1)
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
  
  def include?(obj)
    i = 0
    while i < @total
      test = self[i]
      is = (test == obj)
      return true if is
      i += 1
    end
    
    return false
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
    
  def reverse
    ary = []
    i = @total - 1
    while i >= 0
      ary << self[i]
      i -= 1
    end
    return ary
  end

  def partition
    left = []
    right = []
    i = 0
    while i < @total 
      e = @tuple.at(i)
      yield(e) ? left.push(e) : right.push(e)
      i += 1
    end
    return [left, right]
  end
end
