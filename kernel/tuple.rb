class Tuple
  def self.new(cnt)
    Ruby.primitive :allocate_count  
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
    "#<Tuple: #{join(", ")}>"
  end
  
  def join(sep)
    join_upto(sep, fields)
  end
  
  def join_upto(sep, count)
    str = ""
    return str if count == 0
    count -= 1
    i = 0
    while i < count
      str << at(i).inspect
      str << sep.dup
      i += 1
    end
    str << at(count).inspect
    return str
  end
end