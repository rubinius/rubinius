class Object
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  ivar_as_index :__ivars__ => 0
  
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

  def become!(obj)
    Ruby.primitive :object_become
  end
end

