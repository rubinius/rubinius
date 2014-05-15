class Array
  def self.allocate
    Rubinius.primitive :array_allocate
    raise PrimitiveFailure, "Array.allocate primitive failed"
  end

  def size
    @total
  end

  alias_method :length, :size

  def new_range(start, count)
    Rubinius.primitive :array_new_range
    raise PrimitiveFailure, "Array.new_range primitive failed"
  end

  def new_reserved(count)
    Rubinius.primitive :array_new_reserved
    raise PrimitiveFailure, "Array.new_reserved primitive failed"
  end

  # THIS MUST NOT BE REMOVED. the kernel requires a simple
  # Array#[] to work while parts of the kernel boot.
  def [](idx)
    at(idx)
  end

  def []=(idx, ent)
    Rubinius.check_frozen

    if idx >= @tuple.fields
      new_tuple = Rubinius::Tuple.new(idx + 10)
      new_tuple.copy_from @tuple, @start, @total, 0
      @tuple = new_tuple
    end

    @tuple.put @start + idx, ent
    if idx >= @total - 1
      @total = idx + 1
    end
    return ent
  end

  # Returns the element at the given index. If the
  # index is negative, counts from the end of the
  # Array. If the index is out of range, nil is
  # returned. Slightly faster than +Array#[]+
  def at(idx)
    Rubinius.primitive :array_aref
    idx = Rubinius::Type.coerce_to idx, Fixnum, :to_int

    total = @start + @total

    if idx < 0
      idx += total
    else
      idx += @start
    end

    if idx >= @start and idx < total
      return @tuple.at idx
    end
  end

  # Passes each element in the Array to the given block
  # and returns self.
  def each
    return to_enum(:each) unless block_given?

    i = @start
    total = i + @total
    tuple = @tuple

    while i < total
      yield tuple.at(i)
      i += 1
    end

    self
  end

  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    return to_enum :map unless block_given?
    out = Array.new size

    i = @start
    total = i + @total
    src = @tuple

    dest = Rubinius::Mirror.reflect(out).tuple

    j = 0
    while i < total
      dest[j] = yield src[i]
      i += 1
      j += 1
    end

    out
  end

  # Replaces each element in self with the return value
  # of passing that element to the supplied block.
  def map!
    return to_enum(:map!) unless block_given?

    Rubinius.check_frozen

    i = @start
    total = i + @total
    tuple = @tuple

    while i < total
      tuple[i] = yield tuple.at(i)
      i += 1
    end

    self
  end

  def to_tuple
    tuple = Rubinius::Tuple.new @total
    tuple.copy_from @tuple, @start, @total, 0
    tuple
  end
end
