class Tuple

  def self.new(cnt)
    Ruby.primitive :allocate_count
    raise PrimitiveFailure, "primitive failed"
  end

  def self.template(size, obj)
    Ruby.primitive :tuple_template
    raise PrimitiveFailure, "Tuple.template primitive failed"
  end

  def shifted(distance)
    Ruby.primitive :tuple_shifted
    raise PrimitiveFailure, "primitive failed"
  end

  def [](idx)
    Ruby.primitive :object_at
    raise InvalidIndexError, "Unable to access index '#{idx}' of #{self}"
  end

  def []=(idx, val)
    Ruby.primitive :object_put
    raise InvalidIndexError, "Unable to set index '#{idx}' of #{self}"
  end

  def at(idx)
    Ruby.primitive :object_at
    raise InvalidIndexError, "Tuple#at failed."
  end

  def put(idx, val)
    Ruby.primitive :object_put
    raise InvalidIndexError, "Unable to set index '#{idx}' of #{self}"
  end

  def fields
    Ruby.primitive :fields
    raise PrimitiveFailure, "primitive failed"
  end

end

