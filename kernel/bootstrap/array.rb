class Array
  attr_accessor :total
  attr_accessor :tuple
  attr_accessor :start

  alias_method :size,   :total
  alias_method :length, :total

  def self.allocate
    Ruby.primitive :array_allocate
    raise PrimitiveFailure, "Array.allocate primitive failed"
  end

  # THIS MUST NOT BE REMOVED. the kernel requires a simple
  # Array#[] to work while parts of the kernel boot.
  def [](idx)
    Ruby.primitive :array_aref
  end

  def []=(idx, ent)
    if idx >= @tuple.fields
      nt = Rubinius::Tuple.new(idx + 10)
      nt.copy_from @tuple, @start, @total, 0
      @tuple = nt
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
    Ruby.primitive :array_aref
  end

  # Passes each element in the Array to the given block
  # and returns self.  We re-evaluate @total each time
  # through the loop in case the array has changed.
  def each
    return to_enum :each unless block_given? || Rubinius::TARGET_IS_186
    i = 0
    while i < @total
      yield at(i)
      i += 1
    end
    self
  end

  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    return to_enum :map unless block_given? || Rubinius::TARGET_IS_18
    return dup unless block_given?
    out = Array.new @total
    i = 0
    while i < @total
      out[i] = yield(at(i))
      i += 1
    end
    out
  end

  # Replaces each element in self with the return value
  # of passing that element to the supplied block.
  def map!
    return to_enum :map! unless block_given? || Rubinius::TARGET_IS_186
    i = 0
    while i < @total
      self[i] = yield(at(i))
      i += 1
    end
    self
  end

  # Runtime method to support case when *foo syntax
  def __matches_when__(receiver)
    i = @start
    tot = @total + @start
    while i < tot
      return true if @tuple.at(i) === receiver
      i = i + 1
    end
    false
  end

  def self.coerce_into_array(obj)
    return obj.to_ary if obj.respond_to?(:to_ary)
    [obj]
  end
end
