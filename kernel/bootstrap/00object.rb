class Object
  def extend(*mods)
    (class << self; self; end).include(*mods)
  end

  def at(idx)
    Ruby.primitive :at
    exc = InvalidIndex.new("Could not access index #{idx} of #{self}")
    raise exc
  end
  
  def put(idx, val)
    Ruby.primitive :put
    exc = InvalidIndex.new("Could not write to index #{idx} of #{self}")
    raise exc
  end
  
  def fields
    Ruby.primitive :fields
  end
end

