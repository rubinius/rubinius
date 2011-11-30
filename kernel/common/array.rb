##
# Arrays are ordered, integer-indexed collections of any object.  Array
# indexing starts at 0, as in C or Java.  A negative index is assumed to be
# relative to the end of the array---that is, an index of -1 indicates the
# last element of the array, -2 is the next to last element in the array, and
# so on.
#
# Arrays can be created with the <tt>[]</tt> syntax, or via <tt>Array.new</tt>.

class Array
  include Enumerable

  # The flow control for many of these methods is
  # pretty evil due to how MRI works. There is
  # also a lot of duplication of code due to very
  # subtle processing differences and, in some
  # cases, to avoid mutual dependency. Apologies.


  # Returns a new Array populated with the given objects
  def self.[](*args)
    ary = allocate
    ary.replace args
    ary
  end

  # Creates a new Array. Without arguments, an empty
  # Array is returned. If the only argument is an object
  # that responds to +to_ary+, a copy of that Array is
  # created. Otherwise the first argument is the size
  # of the new Array (default 0.) The second argument
  # may be an object used to fill the Array up to the
  # size given (the same object, not a copy.) Alternatively,
  # a block that takes the numeric index can be given and
  # will be run size times to fill the Array with its
  # result. The block supercedes any object given. If
  # neither is provided, the Array is filled with nil.
  def initialize(size_or_array=undefined, obj=undefined)
    Rubinius.check_frozen

    if size_or_array.equal? undefined
      unless @total == 0
        @total = @start = 0
        @tuple = Rubinius::Tuple.new 8
      end

      return self
    end

    if obj.equal? undefined
      obj = nil

      if size_or_array.respond_to? :to_ary
        ary = Rubinius::Type.coerce_to size_or_array, Array, :to_ary

        @tuple = ary.tuple.dup
        @start = ary.start
        @total = ary.size

        return self
      end
    end

    size = Rubinius::Type.coerce_to size_or_array, Integer, :to_int
    raise ArgumentError, "size must be positive" if size < 0
    raise ArgumentError, "size must be <= #{Fixnum::MAX}" if size > Fixnum::MAX

    if block_given?
      @tuple = Rubinius::Tuple.new size
      @total = i = 0
      while i < size
        @tuple.put i, yield(i)
        @total = i += 1
      end
    else
      @total = size
      @tuple = Rubinius::Tuple.pattern size, obj
    end

    self
  end

  private :initialize

  # Element reference, returns the element at the given index or
  # a subarray starting from the index continuing for length
  # elements or returns a subarray from range elements. Negative
  # indices count from the end. Returns nil if the index or subarray
  # request cannot be completed. Array#slice is synonymous with #[].
  # Subclasses return instances of themselves.
  def [](arg1, arg2=nil)
    case arg1

    # This is split out from the generic case and put first because
    # it is by far the most common case and we want to deal with it
    # immediately, even at the expensive of duplicate code with the
    # generic case below. In other words, don't refactor this unless
    # you preserve the same or better performance.
    when Fixnum
      start_idx = arg1

      # Convert negative indices
      start_idx += @total if start_idx < 0

      if arg2
        count = Rubinius::Type.coerce_to arg2, Fixnum, :to_int
      else
        return nil if start_idx >= @total

        begin
          return @tuple.at(@start + start_idx)

        # Tuple#at raises this if the index is negative or
        # past the end. This is faster than checking explicitly
        # since this is an exceptional case anyway.
        rescue Rubinius::ObjectBoundsExceededError
          return nil
        end
      end
    when Range
      start_idx = Rubinius::Type.coerce_to arg1.begin, Fixnum, :to_int
      # Convert negative indices
      start_idx += @total if start_idx < 0

      # Check here because we must detect this boundary
      # before we check the right index boundary cases
      return nil if start_idx < 0 or start_idx > @total

      right_idx = Rubinius::Type.coerce_to arg1.end, Fixnum, :to_int
      right_idx += @total if right_idx < 0
      right_idx -= 1 if arg1.exclude_end?

      return new_range(0, 0) if right_idx < start_idx

      count = right_idx - start_idx + 1

    # Slower, less common generic coercion case.
    else
      start_idx = Rubinius::Type.coerce_to arg1, Fixnum, :to_int

      # Convert negative indices
      start_idx += @total if start_idx < 0

      if arg2
        count = Rubinius::Type.coerce_to arg2, Fixnum, :to_int
      else
        return nil if start_idx >= @total

        begin
          return @tuple.at(@start + start_idx)

        # Tuple#at raises this if the index is negative or
        # past the end. This is faster than checking explicitly
        # since this is an exceptional case anyway.
        rescue Rubinius::ObjectBoundsExceededError
          return nil
        end
      end
    end

    # No need to go further
    return nil if count < 0

    # Check start boundaries
    if start_idx >= @total
      # Odd MRI boundary case
      return new_range(0, 0) if start_idx == @total
      return nil
    end

    return nil if start_idx < 0

    # Check count boundaries
    if start_idx + count > @total
      count = @total - start_idx
    end

    # Construct the subrange
    return new_range(@start + start_idx, count)
  end

  alias_method :slice, :[]

  # Appends the object to the end of the Array.
  # Returns self so several appends can be chained.
  def <<(obj)
    set_index(@total, obj)
    self
  end

  alias_method :__append__, :<<

  # Creates a new Array containing only elements common to
  # both Arrays, without duplicates. Also known as a 'set
  # intersection'
  def &(other)
    other = Rubinius::Type.coerce_to other, Array, :to_ary

    array = []
    im = Rubinius::IdentityMap.from other

    each { |x| array << x if im.delete x }

    array
  end

  # Creates a new Array by combining the two Arrays' items,
  # without duplicates. Also known as a 'set union.'
  def |(other)
    other = Rubinius::Type.coerce_to other, Array, :to_ary

    im = Rubinius::IdentityMap.from self, other
    im.to_array
  end

  # Create a concatenation of the two Arrays.
  def +(other)
    other = Rubinius::Type.coerce_to other, Array, :to_ary
    Array.new(self).concat(other)
  end

  # Creates a new Array that contains the items of the original
  # Array that do not appear in the other Array, effectively
  # 'deducting' those items. The matching method is Hash-based.
  def -(other)
    other = Rubinius::Type.coerce_to other, Array, :to_ary

    array = []
    im = Rubinius::IdentityMap.from other

    each { |x| array << x unless im.include? x }

    array
  end

  # Compares the two Arrays and returns -1, 0 or 1 depending
  # on whether the first one is 'smaller', 'equal' or 'greater'
  # in relation to the second. Two Arrays are equal only if all
  # their elements are 0 using first_e <=> second_e and their
  # lengths are the same. The element comparison is the primary
  # and length is only checked if the former results in 0's.
  def <=>(other)
    other = Rubinius::Type.check_convert_type other, Array, :to_ary
    return 0 if equal? other
    return nil if other.nil?

    other_total = other.total
    Thread.detect_recursion self, other do
      i = 0
      total = other_total < @total ? other_total : @total

      while i < total
        diff = self[i] <=> other[i]
        return diff unless diff == 0

        i += 1
      end
    end

    # subtle: if we are recursing on that pair, then let's
    # no go any further down into that pair;
    # any difference will be found elsewhere if need be
    @total <=> other_total
  end

  # The two Arrays are considered equal only if their
  # lengths are the same and each of their elements
  # are equal according to first_e == second_e . Both
  # Array subclasses and to_ary objects are accepted.
  def ==(other)
    return true if equal?(other)
    unless other.kind_of? Array
      return false unless other.respond_to? :to_ary
      return other == self
    end

    return false unless size == other.size

    Thread.detect_recursion self, other do
      md = @tuple
      od = other.tuple

      i = @start
      j = other.start

      total = i + @total

      while i < total
        return false unless md[i] == od[j]
        i += 1
        j += 1
      end
    end

    true
  end

  # Assumes the Array contains other Arrays and searches through
  # it comparing the given object with the first element of each
  # contained Array using elem == obj. Returns the first contained
  # Array that matches (the first 'associated' Array) or nil.
  def assoc(obj)
    each do |x|
      if x.kind_of? Array and x.first == obj
        return x
      end
    end

    nil
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

    return nil if idx >= total or idx < @start
    return @tuple.at(idx)
  end

  # Removes all elements in the Array and leaves it empty
  def clear()
    Rubinius.check_frozen

    @tuple = Rubinius::Tuple.new(1)
    @total = 0
    @start = 0
    self
  end

  # When invoked with a block, yields all combinations of length n of elements
  # from ary and then returns ary itself. Even though the implementation makes
  # no guarantees about the order in which the combinations are yielded, we copy MRI.
  # When invoked without a block, returns an enumerator object instead.
  def combination(num)
    num = Rubinius::Type.coerce_to num, Fixnum, :to_int
    return to_enum(:combination, num) unless block_given?
    return self unless (0..size).include? num
    # Implementation note: slightly tricky.
                                             # Example: self = 1..7, num = 3
    picks = (0...num).to_a                   # picks start at 0, 1, 2
    max = ((size-num)...size).to_a           # max (index for a given pick) is [4, 5, 6]
    pick_max_pairs = picks.zip(max).reverse  # pick_max_pairs = [[2, 6], [1, 5], [0, 4]]

    return_proc = Proc.new { return self }
    lookup = pick_max_pairs.find(return_proc)

    while true
      yield values_at(*picks)
      move = lookup.each { |pick, max| picks[pick] < max }.first
      new_index = picks[move] + 1
      picks[move...num] = (new_index...(new_index+num-move)).to_a
    end
  end

  # Removes all nil elements from self, returns nil if no changes
  def compact!
    Rubinius.check_frozen

    if (deleted = @tuple.delete(@start, @total, nil)) > 0
      @total -= deleted
      reallocate_shrink()
      return self
    else
      return nil
    end
  end

  # Calls block for each element repeatedly n times or forever if none
  # or nil is given. If a non-positive number is given or the array is empty,
  # does nothing. Returns nil if the loop has finished without getting interrupted.
  def cycle(n=nil, &block)
    return to_enum(:cycle, n) unless block_given?

    # Don't use nil? because, historically, lame code has overridden that method
    if n.equal? nil
      while true
        each { |x| yield x }
      end
    else
      n = Rubinius::Type.coerce_to n, Fixnum, :to_int
      n.times do
        each { |x| yield x }
      end
    end
    nil
  end

  # Removes all elements from self that are #== to the given object.
  # If the object does not appear at all, nil is returned unless a
  # block is provided in which case the value of running it is
  # returned instead.
  def delete(obj)
    key = undefined
    i = @start
    total = i + @total
    tuple = @tuple

    while i < total
      if tuple.at(i) == obj
        # We MUST check frozen here, not at the top, because MRI
        # requires that #delete not raise unless an element would
        # be deleted.
        Rubinius.check_frozen
        tuple.put i, key
      end
      i += 1
    end

    deleted = @tuple.delete @start, @total, key
    if deleted > 0
      @total -= deleted
      reallocate_shrink()
      return obj
    end

    if block_given?
      yield
    else
      nil
    end
  end

  # Deletes the element at the given index and returns
  # the deleted element or nil if the index is out of
  # range. Negative indices count backwards from end.
  def delete_at(idx)
    Rubinius.check_frozen

    idx = Rubinius::Type.coerce_to idx, Fixnum, :to_int

    # Flip to positive and weed out out of bounds
    idx += @total if idx < 0
    return nil if idx < 0 or idx >= @total

    # Grab the object and adjust the indices for the rest
    obj = @tuple.at(@start + idx)

    # Shift style.
    if idx == 0
      @tuple.put @start, nil
      @start += 1
    else
      @tuple.copy_from(@tuple, @start+idx+1, @total-idx-1, @start+idx)
      @tuple.put(@start + @total - 1, nil)
    end

    @total -= 1
    obj
  end

  # Deletes every element from self for which block evaluates to true
  def delete_if(&block)
    Rubinius.check_frozen

    return to_enum(:delete_if) unless block_given?

    return self if empty?

    i = pos = @start
    total = i + @total
    tuple = @tuple

    while i < total
      x = tuple.at i
      unless yield x
        # Ok, keep the value, so stick it back into the array at
        # the insert position
        tuple.put pos, x
        pos += 1
      end

      i += 1
    end

    @total = pos - @start

    self
  end

  # Passes each index of the Array to the given block
  # and returns self.
  def each_index
    return to_enum(:each_index) unless block_given?

    i = 0
    total = @total

    while i < total
      yield i
      i += 1
    end

    self
  end

  # Returns true if both are the same object or if both
  # have the same elements (#eql? used for testing.)
  def eql?(other)
    return true if equal? other
    return false unless other.kind_of?(Array)
    return false if @total != other.size

    Thread.detect_recursion self, other do
      i = 0
      each do |x|
        return false unless x.eql? other[i]
        i += 1
      end
    end

    true
  end

  # True if Array has no elements.
  def empty?
    @total == 0
  end

  # Attempts to return the element at the given index. By default
  # an IndexError is raised if the element is out of bounds. The
  # user may supply either a default value or a block that takes
  # the index object instead.
  def fetch(idx, default=undefined)
    orig = idx
    idx = Rubinius::Type.coerce_to(idx, Fixnum, :to_int)

    idx += @total if idx < 0

    if idx < 0 or idx >= @total
      if block_given?
        return yield(orig)
      end

      return default unless default.equal? undefined

      raise IndexError, "index #{idx} out of bounds"
    end

    at(idx)
  end

  # Fill some portion of the Array with a given element. The
  # element to be used can be either given as the first argument
  # or as a block that takes the index as its argument. The
  # section that is to be filled can be defined by the following
  # arguments. The first following argument is either a starting
  # index or a Range. If the first argument is a starting index,
  # the second argument can be the length. No length given
  # defaults to rest of Array, no starting defaults to 0. Negative
  # indices are treated as counting backwards from the end. Negative
  # counts leave the Array unchanged. Returns self.
  #
  # array.fill(obj)                                -> array
  # array.fill(obj, start [, length])              -> array
  # array.fill(obj, range)                         -> array
  # array.fill { |index| block }                   -> array
  # array.fill(start [, length]) { |index| block } -> array
  # array.fill(range) { |index| block }            -> array
  def fill(a=undefined, b=undefined, c=undefined)
    Rubinius.check_frozen

    if block_given?
      if undefined != c
        raise ArgumentError, "wrong number of arguments"
      end
      one, two = a, b
    else
      if a.equal? undefined
        raise ArgumentError, "wrong number of arguments"
      end
      obj, one, two = a, b, c
    end

    if one.kind_of? Range
      raise TypeError, "length invalid with range" unless two.equal? undefined

      left = Rubinius::Type.coerce_to one.begin, Fixnum, :to_int
      left += size if left < 0
      raise RangeError, "#{one.inspect} out of range" if left < 0

      right = Rubinius::Type.coerce_to one.end, Fixnum, :to_int
      right += size if right < 0
      right += 1 unless one.exclude_end?
      return self if right <= left           # Nothing to modify

    elsif one and one != undefined
      left = Rubinius::Type.coerce_to one, Fixnum, :to_int
      left += size if left < 0
      left = 0 if left < 0

      if two and two != undefined
        begin
          right = Rubinius::Type.coerce_to two, Fixnum, :to_int
        rescue TypeError
          raise ArgumentError, "second argument must be a Fixnum"
        end

        return self if right == 0
        right += left
      else
        right = size
      end
    else
      left = 0
      right = size
    end

    total = @start + right

    if right > @total
      reallocate total
      @total = right
    end

    # Must be after the potential call to reallocate, since
    # reallocate might change @tuple
    tuple = @tuple

    i = @start + left

    if block_given?
      while i < total
        tuple.put i, yield(i-@start)
        i += 1
      end
    else
      while i < total
        tuple.put i, obj
        i += 1
      end
    end

    self
  end

  # Returns the first or first n elements of the Array.
  # If no argument is given, returns nil if the item
  # is not found. If there is an argument, an empty
  # Array is returned instead.
  def first(n = undefined)
    return at(0) if n.equal? undefined

    n = Rubinius::Type.coerce_to n, Fixnum, :to_int
    raise ArgumentError, "Size must be positive" if n < 0

    self[0, n]
  end

  # Computes a Fixnum hash code for this Array. Any two
  # Arrays with the same content will have the same hash
  # code (similar to #eql?)
  def hash
    hash_val = size
    mask = Fixnum::MAX >> 1

    # This is duplicated and manually inlined code from Thread for performance
    # reasons. Before refactoring it, please benchmark it and compare your
    # refactoring against the original.

    id = object_id
    objects = Thread.current.recursive_objects

    # If there is already an our version running...
    if objects.key? :__detect_outermost_recursion__

      # If we've seen self, unwind back to the outer version
      if objects.key? id
        raise Thread::InnerRecursionDetected
      end

      # .. or compute the hash value like normal
      begin
        objects[id] = true

        each { |x| hash_val = ((hash_val & mask) << 1) ^ x.hash }
      ensure
        objects.delete id
      end

      return hash_val
    else
      # Otherwise, we're the outermost version of this code..
      begin
        objects[:__detect_outermost_recursion__] = true
        objects[id] = true

        each { |x| hash_val = ((hash_val & mask) << 1) ^ x.hash }

        # An inner version will raise to return back here, indicating that
        # the whole structure is recursive. In which case, abondon most of
        # the work and return a simple hash value.
      rescue Thread::InnerRecursionDetected
        return size
      ensure
        objects.delete :__detect_outermost_recursion__
        objects.delete id
      end
    end

    return hash_val
  end

  # Returns true if the given obj is present in the Array.
  # Presence is determined by calling elem == obj until found.
  def include?(obj)

    # This explicit loop is for performance only. Preferably,
    # this method would be implemented as:
    #
    #   each { |x| return true if x == obj }
    #
    # but the JIT will currently not inline the block into the
    # method that calls #include? which causes #include? to
    # execute about 3x slower. Since this is a very commonly
    # used method, this manual performance optimization is used.
    # Ideally, this will be removed when the JIT can handle the
    # block used here.

    i = @start
    total = i + @total
    tuple = @tuple

    while i < total
      return true if tuple.at(i) == obj
      i += 1
    end

    false
  end

  # Returns the index of the first element in the Array
  # for which elem == obj is true or nil. If a block is
  # given instead of an argument, returns first object
  # for which block is true. Returns nil if no match is found.
  def index(obj=undefined)
    i = 0
    if obj.equal? undefined
      each do |x|
        return i if yield(x)
        i += 1
      end
    else
      each do |x|
        return i if x == obj
        i += 1
      end
    end
    nil
  end

  # Returns an Array populated with the objects at the given indices of the original.
  # Range arguments are given as nested Arrays as from #[].
  def indexes(*args)
    warn 'Array#indexes is deprecated, use Array#values_at instead'

    out = []

    args.each do |a|
      if a.kind_of? Range
        out << self[a]
      else
        idx = Rubinius::Type.coerce_to(a, Fixnum, :to_int)
        out << at(idx)
      end
    end

    out
  end

  alias_method :indices, :indexes

  # Returns the last element or n elements of self. If
  # the Array is empty, without a count nil is returned,
  # otherwise an empty Array. Always returns an Array.
  def last(n=undefined)
    if size < 1
      return nil if n.equal? undefined
      return []
    end

    return at(-1) if n.equal? undefined

    n = Rubinius::Type.coerce_to n, Fixnum, :to_int
    return [] if n == 0

    raise ArgumentError, "count must be positive" if n < 0

    n = size if n > size
    self[-n..-1]
  end

  alias_method :collect, :map

  alias_method :collect!, :map!

  # Returns number of non-nil elements in self, may be zero
  def nitems
    sum = 0
    each { |elem| sum += 1 unless elem.equal? nil }
    sum
  end

  ##
  #  call-seq:
  #     ary.permutation { |p| block }          -> array
  #     ary.permutation                        -> enumerator
  #     ary.permutation(n) { |p| block }       -> array
  #     ary.permutation(n)                     -> enumerator
  #
  #  When invoked with a block, yield all permutations of length <i>n</i>
  #  of the elements of <i>ary</i>, then return the array itself.
  #  If <i>n</i> is not specified, yield all permutations of all elements.
  #  The implementation makes no guarantees about the order in which
  #  the permutations are yielded.
  #
  #  When invoked without a block, return an enumerator object instead.
  #
  #  Examples:
  #
  #     a = [1, 2, 3]
  #     a.permutation.to_a     #=> [[1,2,3],[1,3,2],[2,1,3],[2,3,1],[3,1,2],[3,2,1]]
  #     a.permutation(1).to_a  #=> [[1],[2],[3]]
  #     a.permutation(2).to_a  #=> [[1,2],[1,3],[2,1],[2,3],[3,1],[3,2]]
  #     a.permutation(3).to_a  #=> [[1,2,3],[1,3,2],[2,1,3],[2,3,1],[3,1,2],[3,2,1]]
  #     a.permutation(0).to_a  #=> [[]] # one permutation of length 0
  #     a.permutation(4).to_a  #=> []   # no permutations of length 4
  #
  def permutation(num=undefined, &block)
    return to_enum(:permutation, num) unless block_given?

    if num.equal? undefined
      num = @total
    else
      num = Rubinius::Type.coerce_to num, Fixnum, :to_int
    end

    if num < 0 || @total < num
      # no permutations, yield nothing
    elsif num == 0
      # exactly one permutation: the zero-length array
      yield []
    elsif num == 1
      # this is a special, easy case
      each { |val| yield [val] }
    else
      # this is the general case
      perm = Array.new(num)
      used = Array.new(@total, false)

      if block
        # offensive (both definitions) copy.
        offensive = dup
        Rubinius.privately do
          offensive.__permute__(num, perm, 0, used, &block)
        end
      else
        __permute__(num, perm, 0, used, &block)
      end
    end

    self
  end

  def __permute__(num, perm, index, used, &block)
    # Recursively compute permutations of r elements of the set [0..n-1].
    # When we have a complete permutation of array indexes, copy the values
    # at those indexes into a new array and yield that array.
    #
    # num: the number of elements in each permutation
    # p: the array (of size num) that we're filling in
    # index: what index we're filling in now
    # used: an array of booleans: whether a given index is already used
    #
    # Note: not as efficient as could be for big num.
    @total.times do |i|
      unless used[i]
        perm[index] = i
        if index < num-1
          used[i] = true
          __permute__(num, perm, index+1, used, &block)
          used[i] = false
        else
          yield values_at(*perm)
        end
      end
    end
  end
  private :__permute__

  # Removes and returns the last element from the Array.
  def pop(many=undefined)
    Rubinius.check_frozen

    if many.equal? undefined
      return nil if @total == 0

      @total -= 1
      index = @start + @total

      elem = @tuple.at(index)
      @tuple.put index, nil

      elem
    else
      many = Rubinius::Type.coerce_to(many, Fixnum, :to_int)
      raise ArgumentError, "negative array size" if many < 0

      first = @total - many
      first = 0 if first < 0

      out = self[first, many]

      if many > @total
        @total = 0
      else
        @total -= many
      end

      return out
    end
  end

  # Searches through contained Arrays within the Array,
  # comparing obj with the second element of each using
  # elem == obj. Returns the first matching Array, nil
  # on failure. See also Array#assoc.
  def rassoc(obj)
    each do |elem|
      if elem.kind_of? Array and elem.at(1) == obj
        return elem
      end
    end

    nil
  end

  # Equivalent to #delete_if except that returns nil if
  # no changes were made.
  def reject!(&block)
    Rubinius.check_frozen

    return to_enum(:reject!) unless block_given?

    was = size()
    delete_if(&block)

    return nil if was == size()
    self
  end

  # Returns a new Array or subclass populated from self
  # but in reverse order.
  def reverse
    dup.reverse!
  end

  # Reverses the order of elements in self. Returns self
  # even if no changes are made
  def reverse!
    Rubinius.check_frozen

    return self unless @total > 1

    @tuple.reverse! @start, @total

    return self
  end

  # Goes through the Array back to front and yields
  # each element to the supplied block. Returns self.
  def reverse_each
    return to_enum(:reverse_each) unless block_given?

    stop = @start - 1
    i = stop + @total
    tuple = @tuple

    while i > stop
      yield tuple.at(i)
      i -= 1
    end

    self
  end

  # Returns the index of the last element in the Array
  # for which elem == obj is true.
  # If a block is given instead of an argument,
  # returns last object for which block is true.
  def rindex(obj=undefined)
    if obj.equal? undefined
      return to_enum(:rindex, obj) unless block_given?

      i = @total - 1
      while i >= 0
        return i if yield @tuple.at(@start + i)

        # Compensate for the array being modified by the block
        i = @total if i > @total

        i -= 1
      end
    else
      stop = @start - 1
      i = stop + @total
      tuple = @tuple

      while i > stop
        return i if tuple.at(i) == obj
        i -= 1
      end
    end
    nil
  end

  # Some code depends on Array having it's own #select method,
  # not just using the Enumerable one. This alias achieves that.
  alias_method :select, :find_all

  # Removes and returns the first element in the
  # Array or nil if empty. All other elements are
  # moved down one index.
  def shift(n=undefined)
    Rubinius.check_frozen

    if n.equal? undefined
      return nil if @total == 0
      obj = @tuple.at @start
      @tuple.put @start, nil
      @start += 1
      @total -= 1

      obj
    else
      n = Rubinius::Type.coerce_to(n, Fixnum, :to_int)
      raise ArgumentError, "negative array size" if n < 0

      slice!(0, n)
    end
  end

  ##
  #  call-seq:
  #     ary.drop(n)               => array
  #
  #  Drops first n elements from <i>ary</i>, and returns rest elements
  #  in an array.
  #
  #     a = [1, 2, 3, 4, 5, 0]
  #     a.drop(3)             # => [4, 5, 0]
  #
  #   enum.drop(n)   => an_array
  #
  # This a specialized version of the method found in Enumerable
  def drop(n)
    n = Rubinius::Type.coerce_to(n, Fixnum, :to_int)
    raise ArgumentError, "attempt to drop negative size" if n < 0

    return [] if @total == 0

    new_size = @total - n
    return [] if new_size <= 0

    new_range n, new_size
  end

  # Shuffles elements in self in place.
  def shuffle!
    Rubinius.check_frozen

    size.times do |i|
      r = i + Kernel.rand(size - i)
      @tuple.swap(i, r)
    end
    self
  end

  # Returns a new Array created by sorting elements in self. Comparisons
  # to sort are either done using <=> or a block may be provided. The
  # block must take two parameters and return -1, 0, 1 depending on
  # whether the first parameter is smaller, equal or larger than the
  # second parameter.
  def sort(&block)
    dup.sort_inplace(&block)
  end

  # Sorts this Array in-place. See #sort.
  #
  # The threshold for choosing between Insertion sort and Quicksort
  # is 10, as determined by a bit of quick tests.
  # Solving directly for recurrence relations is impossible, since quicksort
  # depends on the partition sizes, which is dependent upon the arrangement
  # of the array.
  #
  # For results and methodology, see the commit message.
  def sort_inplace(&block)
    Rubinius.check_frozen

    return self unless @total > 1

    if (@total - @start) < 13
      if block
        isort_block! @start, (@start + @total) - 1, block
      else
        isort! @start, (@start + @total) - 1
      end
    else
      if block
        qsort_block! block
      else
        qsort!
      end
    end

    self
  end

  protected :sort_inplace

  # Make a public alias. Only use sort_inplace internally to avoid
  # subclass overrides.

  # Alias used to avoid subclass overrides
  alias_method :sort!, :sort_inplace
  public :sort!

  # Returns self except on subclasses which are converted
  # or 'upcast' to Arrays.
  def to_a
    if self.instance_of? Array
      self
    else
      Array.new(self)
    end
  end

  # Returns self
  def to_ary
    self
  end

  # Treats all elements as being Arrays of equal lengths and
  # transposes their rows and columns so that the first contained
  # Array in the result includes the first elements of all the
  # Arrays and so on.
  def transpose
    return [] if empty?

    out = []
    max = nil

    each do |ary|
      ary = Rubinius::Type.coerce_to ary, Array, :to_ary
      max ||= ary.size

      # Catches too-large as well as too-small (for which #fetch would suffice)
      raise IndexError, "All arrays must be same length" if ary.size != max

      ary.size.times do |i|
        entry = (out[i] ||= [])
        entry << ary.at(i)
      end
    end

    out
  end

  # Returns a new Array populated from the elements in
  # self corresponding to the given selector(s.) The
  # arguments may be one or more integer indices or
  # Ranges.
  def values_at(*args)
    out = []

    args.each do |elem|
      # Cannot use #[] because of subtly different errors
      if elem.kind_of? Range
        finish = Rubinius::Type.coerce_to elem.last, Fixnum, :to_int
        start = Rubinius::Type.coerce_to elem.first, Fixnum, :to_int

        start += @total if start < 0
        next if start < 0

        finish += @total if finish < 0
        finish -= 1 if elem.exclude_end?
        finish = @total unless finish < @total

        next if finish < start

        start.upto(finish) { |i| out << at(i) }

      else
        i = Rubinius::Type.coerce_to elem, Fixnum, :to_int
        out << at(i)
      end
    end

    out
  end

  # Reallocates the internal Tuple to accommodate at least given size
  def reallocate(at_least)
    return if at_least < @tuple.size

    new_total = @tuple.size * 2

    if new_total < at_least
      new_total = at_least
    end

    new_tuple = Rubinius::Tuple.new new_total
    new_tuple.copy_from @tuple, @start, @total, 0

    @start = 0
    @tuple = new_tuple
  end

  private :reallocate

  def reallocate_shrink
    new_total = @tuple.size
    return if @total > (new_total / 3)

    # halve the tuple size until the total > 1/3 the size of the total
    begin
      new_total /= 2
    end while @total < (new_total / 6)

    new_tuple = Rubinius::Tuple.new(new_total)
    # position values in the middle somewhere
    new_start = (new_total - @total)/2
    new_tuple.copy_from @tuple, @start, @total, new_start

    @start = new_start
    @tuple = new_tuple
  end

  private :reallocate_shrink

  # Helper to recurse through flattening since the method
  # is not allowed to recurse itself. Detects recursive structures.
  def recursively_flatten(array, out, max_levels = -1)
    modified = false

    # Strict equality since < 0 means 'infinite'
    if max_levels == 0
      out.concat(array)
      return false
    end

    max_levels -= 1
    recursion = Thread.detect_recursion(array) do
      i = array.start
      total = i + array.total
      tuple = array.tuple

      while i < total
        o = tuple.at i

        if ary = Rubinius::Type.check_convert_type(o, Array, :to_ary)
          modified = true
          recursively_flatten(ary, out, max_levels)
        else
          out << o
        end

        i += 1
      end
    end

    raise ArgumentError, "tried to flatten recursive array" if recursion
    modified
  end

  private :recursively_flatten

  # In-place non-recursive sort between the given indexes.
  def qsort!
    stack = [[@start, @start + @total - 1]]

    until stack.empty?
      left, right = stack.pop

      if right > left
        pivotindex = left + ((right - left) / 2)
        # inline pivot routine

        pivot = @tuple.at(pivotindex)

        @tuple.swap(pivotindex, right)
        store = left

        i = left
        while i < right
          cmp = (@tuple.at(i) <=> pivot)
          if cmp < 0
            @tuple.swap(i, store)
            store += 1
          end

          i += 1
        end

        @tuple.swap(store, right)

        pi_new = store

        # end pivot
        stack.push [left, pi_new - 1]
        stack.push [pi_new + 1, right]
      end
    end

    self
  end
  private :qsort!

  # In-place non-recursive sort between the given indexes using a block.
  def qsort_block!(block)
    stack = [[@start, @start + @total - 1]]

    until stack.empty?
      left, right = stack.pop

      if right > left
        pivotindex = left + ((right - left) / 2)
        # pi_new = qsort_partition(left, right, pi)
        # inline pivot routine

        pivot = @tuple.at(pivotindex)

        @tuple.swap(pivotindex, right)
        store = left

        i = left
        while i < right
          block_result = block.call(@tuple.at(i), pivot)
          raise ArgumentError, 'block returned nil' if block_result.nil?
          cmp = Comparable.compare_int block_result
          if cmp < 0
            @tuple.swap(i, store)
            store += 1
          end

          i += 1
        end

        @tuple.swap(store, right)

        pi_new = store

        # end pivot
        stack.push [left, pi_new - 1]
        stack.push [pi_new + 1, right]
      end
    end

    self
  end
  private :qsort_block!

  # Insertion sort in-place between the given indexes.
  def isort!(left, right)
    i = left + 1

    tup = @tuple

    while i <= right
      j = i

      while j > @start
        jp = j - 1
        el1 = tup.at(jp)
        el2 = tup.at(j)

        unless cmp = (el1 <=> el2)
          raise ArgumentError, "comparison of #{el1.inspect} with #{el2.inspect} failed (#{j})"
        end

        break unless cmp > 0

        tup.put(j, el1)
        tup.put(jp, el2)

        j = jp
      end

      i += 1
    end
  end
  private :isort!

  # Insertion sort in-place between the given indexes using a block.
  def isort_block!(left, right, block)
    i = left + 1

    while i <= right
      j = i

      while j > @start
        block_result = block.call(@tuple.at(j - 1), @tuple.at(j))

        if block_result.nil?
          raise ArgumentError, 'block returnd nil'
        elsif block_result > 0
          @tuple.swap(j, (j - 1))
          j -= 1
        else
          break
        end
      end

      i += 1
    end
  end
  private :isort_block!

  # Move to compiler runtime
  def __rescue_match__(exception)
    each { |x| return true if x === exception }
    false
  end

  # Custom API support to support Enumerator#next with :each
  class ValueGenerator
    def initialize(array)
      @array = array
      @index = 0
    end

    def next?
      @index < @array.size
    end

    def next
      val = @array.at(@index)
      @index += 1
      return val
    end

    def rewind
      @index = 0
    end
  end

  def to_generator(method)
    if method == :each
      ValueGenerator.new(self)
    end
  end
end
