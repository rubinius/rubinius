class Array

  # Iterator encapsulates accessing each nth element of an array, where n
  # defaults to 1. The forward iterator always returns the first element in
  # the array and each nth element thereafter. The reverse iterator always
  # returns the last element in the array and each nth previous element
  # thereafter.
  #
  # Calls to #item and #index are only valid after #next or #rnext have
  # returned true.
  #
  # Example usage:
  #
  #   a = [1, 2, 3, 4]
  #
  #   out = []
  #   i = a.to_iter 2
  #   while i.next
  #     out << [i.index, i.item]
  #   end
  #
  #   # => [[0, 1], [2, 3]]
  #
  #   out = []
  #   i = a.to_reverse_iter
  #   while i.rnext
  #     out << i.item
  #   end
  #   p out
  #
  #   # => [4, 3, 2, 1]
  class Iterator
    def self.allocate
      raise TypeError, "Array::Iterator cannot be created via allocate()"
    end

    # Creates a new iterator for +array+ that advances by +step+ increments.
    # If +reverse+ is false, the iterator starts at the first element and
    # advances toward the last. If +reverse+ is true, the iterator starts at
    # the last element and advances toward the first.
    def self.new(array, step, reverse)
      Ruby.primitive :array_iterator_allocate
      raise PrimitiveFailure, "Array::Iterator.new primitive failed"
    end

    # Advances the iterator toward the last element of the array by +step+
    # increment. Returns true if an element of the array can be retrieve with
    # #item, false otherwise.
    def next
      Ruby.primitive :array_iterator_next
      raise PrimitiveFailure, "Array::Iterator#next primitive failed"
    end

    # Advances the iterator toward the first element of the array by +step+
    # increment. Returns true if an element of the array can be accessed with
    # #item, false otherwise.
    def rnext
      Ruby.primitive :array_iterator_rnext
      raise PrimitiveFailure, "Array::Iterator#rnext primitive failed"
    end

    # Returns the element of the array to which the iterator is currently
    # pointing.
    def item
      Ruby.primitive :array_iterator_item
      raise PrimitiveFailure, "Array::Iterator#item primitive failed"
    end

    # Returns an element of the array relative to the element to which the
    # iterator is currently pointing. If +index+ is 0, returns the same
    # element as #item. If +index+ is positive, returns the +index+th element
    # toward the last element from the current element. If +index+ is
    # positive, returns the +index+th element toward the first element from
    # the current element.
    def at(index)
      Ruby.primitive :array_iterator_at
      raise PrimitiveFailure, "Array::Iterator#at primitive failed"
    end

    # Returns the index of the element of the array to which the iterator is
    # currently pointing.
    def index
      Ruby.primitive :array_iterator_index
      raise PrimitiveFailure, "Array::Iterator#index primitive failed"
    end
  end

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
    at(idx)
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
    idx = @total - idx if idx < 0
    return nil if idx > @total
    @tuple[@start + idx]
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

  def to_iter(step=1)
    Iterator.new self, step, false
  end

  def to_reverse_iter(step=1)
    Iterator.new self, step, true
  end
end
