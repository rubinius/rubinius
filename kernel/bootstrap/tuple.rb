class Tuple

  def self.new(cnt)
    Ruby.primitive :allocate_count
    raise PrimitiveFailure, "primitive failed"
  end
  
  def shifted(distance)
    Ruby.primitive :tuple_shifted
    raise PrimitiveFailure, "primitive failed"
  end
  
  def [](idx)
    Ruby.primitive :at
    raise InvalidIndexError, "Unable to access index '#{idx}' of #{self}"
  end

  def []=(idx, val)
    Ruby.primitive :put
    raise InvalidIndexError, "Unable to access index '#{idx}' of #{self}"
  end
  
  def at(idx)
    Ruby.primitive :at
    raise InvalidIndexError, "Object#at failed."
  end

  def fields
    Ruby.primitive :fields
    raise PrimitiveFailure, "primitive failed"
  end

end

