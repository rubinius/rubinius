class Tuple
  
  def self.[](*args)
    sz = args.size
    tup = new(sz)
    i = 0
    while i < sz
      tup.put i, args[i]
      i += 1
    end
    
    return tup
  end
    
  def dup
    tup = Tuple.new(self.fields)
    tup.copy_from(self, 0)
    return tup
  end

  def to_s
    "#<Tuple:0x#{object_id.to_s(16)} #{fields} elements>"
  end
  
  def each
    i = 0
    t = fields
    while i < t
      yield self.at(i)
      i += 1
    end
    self
  end
  
  def inspect
    str = "#<Tuple"
    if fields != 0
      str << ": #{join(", ", :inspect)}"
    end
    str << ">"
    return str
  end
  
  def join(sep, meth=:to_s)
    join_upto(sep, fields, meth)
  end
  
  def join_upto(sep, count, meth=:to_s)
    str = ""
    return str if count == 0 or empty?
    count = fields if count >= fields
    count -= 1
    i = 0
    while i < count
      str << at(i).__send__(meth)
      str << sep.dup
      i += 1
    end
    str << at(count).__send__(meth)
    return str
  end
  
  def to_a
    ary = []
    each do |ent|
      ary << ent unless ent.nil?
    end
    return ary
  end
  
  def shift
    return self unless fields > 0
    t = Tuple.new(fields-1)
    t.copy_from self, 1
    return t
  end
  
  alias :size :fields
  alias :length :fields
  
  def empty?
    size == 0
  end
  
  def first
    at(0)
  end
  
  def last
    at(fields-1)
  end
end
