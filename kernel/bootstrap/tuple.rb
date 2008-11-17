class Tuple

  def self.new(cnt)
    Ruby.primitive :tuple_allocate
    raise PrimitiveFailure, "primitive failed"
  end

  def self.pattern(size, obj)
    Ruby.primitive :tuple_pattern
    raise PrimitiveFailure, "Tuple.pattern primitive failed"
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
    raise InvalidIndexError, "Unable to access index '#{idx}' of #{self}"
  end

  def put(idx, val)
    Ruby.primitive :tuple_put
    raise InvalidIndexError, "Unable to set index '#{idx}' of #{self}"
  end

  def fields
    Ruby.primitive :tuple_fields
    raise PrimitiveFailure, "primitive failed"
  end

  def copy_from(other, start, length, dest)
    Ruby.primitive :tuple_copy_from
    raise PrimitiveFailure, "Tuple#copy_from primitive failed"
  end

  def self.create_weakref(object)
    Ruby.primitive :tuple_create_weakref
    raise PrimitiveFailure, "Unable to create a weak reference"
  end
end
