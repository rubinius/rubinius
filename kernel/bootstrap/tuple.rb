class Tuple

  def self.new(cnt)
    Ruby.primitive :tuple_allocate
    raise PrimitiveFailure, "primitive failed"
  end

  def shifted(distance)
    Ruby.primitive :tuple_shifted
    raise PrimitiveFailure, "primitive failed"
  end

  def [](idx)
    Ruby.primitive :tuple_at
    raise InvalidIndexError, "Unable to access index '#{idx}' of #{self}"
  end

  def []=(idx, val)
    Ruby.primitive :tuple_put
    raise InvalidIndexError, "Unable to set index '#{idx}' of #{self}"
  end

  def at(idx)
    Ruby.primitive :tuple_at
    raise InvalidIndexError, "Object#at failed."
  end

  def put(idx, val)
    Ruby.primitive :tuple_put
    raise InvalidIndexError, "Unable to set index '#{idx}' of #{self}"
  end

  def fields
    Ruby.primitive :tuple_fields
    raise PrimitiveFailure, "primitive failed"
  end

end

