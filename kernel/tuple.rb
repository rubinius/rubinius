class Tuple
  def self.new(cnt)
    Ruby.primitive :allocate_count
  end
  
  def dup
    tup = Tuple.new(self.fields)
    tup.copy_from(self, 0)
    return tup
  end

  def shifted(distance)
    Ruby.primitive :tuple_shifted
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
    return str if count == 0
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
    t = Tuple.new(fields-1)
    t.copy_from self, 1
    return t
  end
end
