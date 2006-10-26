class Array
  def to_s
    "#<Array:0x#{object_id.to_s(16)} #{total} elements>"
  end
  
  def inspect
    "[#{join(", ", :inspect)}]"
  end
  
  def each
    t = self.total
    i = 0
    while i < t
      yield tuple.at(i)
      i += 1
    end
    self
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
    t = self.total
    return str if t == 0
    tuple.join_upto(sep, t, meth)
  end
  
  def <<(ent)
    self[total] = ent
  end
  
  def [](idx)
    if idx >= total
      return nil
    end
    
    tuple.at(idx)
  end
  
  def []=(idx, ent)
    use = tuple
    if idx >= use.fields
      nt = Tuple.new(idx + 10)
      nt.copy_from use, 0
      put 1, nt
      use = nt
    end

    use.put idx, ent
    put(0, idx + 1)
    return ent
  end
  
  def +(other)
    out = dup()
    other.each { |e| out << e }
    return out
  end
  
  def dup
    ary = []
    each { |e| ary << e }
    return ary
  end
  
  def include?(obj)
    nd = self.total
    i = 0
    while i < nd
      test = self[i]
      is = (test == obj)
      return true if is
      i += 1
    end
    
    return false
  end
  
  def shift
    ele = self[0]
    return ele unless ele
    
    out = tuple.shift
    
    put 1, out
    put(0, total - 1)
    return ele
  end
  
  def reverse
    ary = []
    i = self.total - 1
    while i >= 0
      ary << self[i]
      i -= 1
    end
    return ary
  end
end