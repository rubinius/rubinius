# -*- encoding: us-ascii -*-

class Array
  include Enumerable

  # The flow control for many of these methods is
  # pretty evil due to how MRI works. There is
  # also a lot of duplication of code due to very
  # subtle processing differences and, in some
  # cases, to avoid mutual dependency. Apologies.


  def self.[](*args)
    ary = allocate
    ary.replace args
    ary
  end

  def initialize(size_or_array=undefined, obj=undefined)
    Rubinius.check_frozen

    if undefined.equal?(size_or_array)
      unless @total == 0
        @total = @start = 0
        @tuple = Rubinius::Tuple.new 8
      end

      return self
    end

    if undefined.equal?(obj)
      obj = nil
      ary = nil
      if size_or_array.kind_of? Integer
        # Do nothing, fall through to later case.
      elsif size_or_array.kind_of? Array
        ary = size_or_array
      elsif size_or_array.respond_to? :to_ary
        ary = Rubinius::Type.coerce_to size_or_array, Array, :to_ary
      end

      if ary
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

  # Replaces contents of self with contents of other,
  # adjusting size as needed.
  def replace(other)
    Rubinius.check_frozen

    other = Rubinius::Type.coerce_to other, Array, :to_ary

    @tuple = other.tuple.dup
    @total = other.total
    @start = other.start

    Rubinius::Type.infect(self, other)
    self
  end

  alias_method :initialize_copy, :replace
  private :initialize_copy

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

  def <<(obj)
    set_index(@total, obj)
    self
  end

  alias_method :__append__, :<<

  def &(other)
    other = Rubinius::Type.coerce_to other, Array, :to_ary

    array = []
    im = Rubinius::IdentityMap.from other

    each { |x| array << x if im.delete x }

    array
  end

  def |(other)
    other = Rubinius::Type.coerce_to other, Array, :to_ary

    im = Rubinius::IdentityMap.from self, other
    im.to_array
  end

  def +(other)
    other = Rubinius::Type.coerce_to other, Array, :to_ary
    Array.new(self).concat(other)
  end

  def -(other)
    other = Rubinius::Type.coerce_to other, Array, :to_ary

    array = []
    im = Rubinius::IdentityMap.from other

    each { |x| array << x unless im.include? x }

    array
  end

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

  def assoc(obj)
    each do |x|
      if x.kind_of? Array and x.first == obj
        return x
      end
    end

    nil
  end

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

  def clear
    Rubinius.check_frozen

    @tuple = Rubinius::Tuple.new(1)
    @total = 0
    @start = 0
    self
  end

  def combination(num)
    num = Rubinius::Type.coerce_to num, Fixnum, :to_int
    return to_enum(:combination, num) unless block_given?

    if num == 0
      yield []
    elsif num == 1
      each do |i|
        yield [i]
      end
    elsif num == size
      yield self.dup
    elsif num >= 0 && num < size
      stack = Rubinius::Tuple.pattern num + 1, 0
      chosen = Rubinius::Tuple.new num
      lev = 0
      done = false
      stack[0] = -1
      until done
        chosen[lev] = self.at(stack[lev+1])
        while lev < num - 1
          lev += 1
          chosen[lev] = self.at(stack[lev+1] = stack[lev] + 1)
        end
        yield chosen.to_a
        lev += 1
        begin
          done = lev == 0
          stack[lev] += 1
          lev -= 1
        end while stack[lev+1] + num == size + lev + 1
      end
    end
    self
  end

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

  def cycle(n=nil)
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

  def delete_if
    return to_enum(:delete_if) unless block_given?

    Rubinius.check_frozen

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

  def empty?
    @total == 0
  end

  def fetch(idx, default=undefined)
    orig = idx
    idx = Rubinius::Type.coerce_to(idx, Fixnum, :to_int)

    idx += @total if idx < 0

    if idx < 0 or idx >= @total
      if block_given?
        return yield(orig)
      end

      return default unless undefined.equal?(default)

      raise IndexError, "index #{idx} out of bounds"
    end

    at(idx)
  end

  def fill(a=undefined, b=undefined, c=undefined)
    Rubinius.check_frozen

    if block_given?
      unless undefined.equal?(c)
        raise ArgumentError, "wrong number of arguments"
      end
      one, two = a, b
    else
      if undefined.equal?(a)
        raise ArgumentError, "wrong number of arguments"
      end
      obj, one, two = a, b, c
    end

    if one.kind_of? Range
      raise TypeError, "length invalid with range" unless undefined.equal?(two)

      left = Rubinius::Type.coerce_to one.begin, Fixnum, :to_int
      left += size if left < 0
      raise RangeError, "#{one.inspect} out of range" if left < 0

      right = Rubinius::Type.coerce_to one.end, Fixnum, :to_int
      right += size if right < 0
      right += 1 unless one.exclude_end?
      return self if right <= left           # Nothing to modify

    elsif one and !undefined.equal?(one)
      left = Rubinius::Type.coerce_to one, Fixnum, :to_int
      left += size if left < 0
      left = 0 if left < 0

      if two and !undefined.equal?(two)
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

  def first(n = undefined)
    return at(0) if undefined.equal?(n)

    n = Rubinius::Type.coerce_to n, Fixnum, :to_int
    raise ArgumentError, "Size must be positive" if n < 0

    Array.new self[0, n]
  end

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

  def find_index(obj=undefined)
    super
  end

  alias_method :index, :find_index

  def last(n=undefined)
    if undefined.equal?(n)
      return at(-1)
    elsif size < 1
      return []
    end

    n = Rubinius::Type.coerce_to n, Fixnum, :to_int
    return [] if n == 0

    raise ArgumentError, "count must be positive" if n < 0

    n = size if n > size
    Array.new self[-n..-1]
  end

  alias_method :collect, :map

  alias_method :collect!, :map!

  def nitems
    sum = 0
    each { |elem| sum += 1 unless elem.equal? nil }
    sum
  end

  def permutation(num=undefined, &block)
    return to_enum(:permutation, num) unless block_given?

    if undefined.equal? num
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

  def pop(many=undefined)
    Rubinius.check_frozen

    if undefined.equal?(many)
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

      out = Array.new self[first, many]

      if many > @total
        @total = 0
      else
        @total -= many
      end

      return out
    end
  end

  def rassoc(obj)
    each do |elem|
      if elem.kind_of? Array and elem.at(1) == obj
        return elem
      end
    end

    nil
  end

  def reject!(&block)
    return to_enum(:reject!) unless block_given?

    Rubinius.check_frozen

    was = size()
    delete_if(&block)

    return nil if was == size()
    self
  end

  def reverse!
    Rubinius.check_frozen

    return self unless @total > 1

    @tuple.reverse! @start, @total

    return self
  end

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

  def rindex(obj=undefined)
    if undefined.equal?(obj)
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
        return i - @start if tuple.at(i) == obj
        i -= 1
      end
    end
    nil
  end

  # Some code depends on Array having it's own #select method,
  # not just using the Enumerable one. This alias achieves that.
  alias_method :select, :find_all

  def shift(n=undefined)
    Rubinius.check_frozen

    if undefined.equal?(n)
      return nil if @total == 0
      obj = @tuple.at @start
      @tuple.put @start, nil
      @start += 1
      @total -= 1

      obj
    else
      n = Rubinius::Type.coerce_to(n, Fixnum, :to_int)
      raise ArgumentError, "negative array size" if n < 0

      Array.new slice!(0, n)
    end
  end

  def drop(n)
    n = Rubinius::Type.coerce_to(n, Fixnum, :to_int)
    raise ArgumentError, "attempt to drop negative size" if n < 0

    return [] if @total == 0

    new_size = @total - n
    return [] if new_size <= 0

    new_range n, new_size
  end

  # Sorts this Array in-place. See #sort.
  #
  # The threshold for choosing between Insertion sort and Mergesort
  # is 13, as determined by a bit of quick tests.
  #
  # For results and methodology, see the commit message.
  def sort_inplace(&block)
    Rubinius.check_frozen

    return self unless @total > 1

    if (@total - @start) < 13
      if block
        isort_block! @start, (@start + @total), block
      else
        isort! @start, (@start + @total)
      end
    else
      if block
        mergesort_block! block
      else
        mergesort!
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

  def to_a
    if self.instance_of? Array
      self
    else
      Array.new(self)
    end
  end

  def to_ary
    self
  end

  def inspect
    return "[]" if @total == 0

    comma = ", "
    result = "["

    return "[...]" if Thread.detect_recursion self do
      each { |o| result << o.inspect << comma }
    end

    Rubinius::Type.infect(result, self)
    result.shorten!(2)
    result << "]"
  end

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

  # Non-recursive sort using a temporary tuple for scratch storage.
  # This is a hybrid mergesort; it's hybrid because for short runs under
  # 8 elements long we use insertion sort and then merge those sorted
  # runs back together.
  def mergesort!
    width = 7
    @scratch = Rubinius::Tuple.new @tuple.size

    # do a pre-loop to create a bunch of short sorted runs; isort on these
    # 7-element sublists is more efficient than doing merge sort on 1-element
    # sublists
    left = @start
    finish = @total + @start
    while left < finish
      right = left + width
      right = right < finish ? right : finish
      last = left + (2 * width)
      last = last < finish ? last : finish

      isort!(left, right)
      isort!(right, last)

      left += 2 * width
    end

    # now just merge together those sorted lists from the prior loop
    width = 7
    while width < @total
      left = @start
      while left < finish
        right = left + width
        right = right < finish ? right : finish
        last = left + (2 * width)
        last = last < finish ? last : finish

        bottom_up_merge(left, right, last)
        left += 2 * width
      end

      @tuple, @scratch = @scratch, @tuple
      width *= 2
    end

    @scratch = nil
    self
  end
  private :mergesort!

  def bottom_up_merge(left, right, last)
    left_index = left
    right_index = right
    i = left

    while i < last
      if left_index < right && (right_index >= last || (@tuple.at(left_index) <=> @tuple.at(right_index)) <= 0)
        @scratch[i] = @tuple.at(left_index)
        left_index += 1
      else
        @scratch[i] = @tuple.at(right_index)
        right_index += 1
      end

      i += 1
    end
  end
  private :bottom_up_merge

  def mergesort_block!(block)
    width = 7
    @scratch = Rubinius::Tuple.new @tuple.size

    left = @start
    finish = @total + @start
    while left < finish
      right = left + width
      right = right < finish ? right : finish
      last = left + (2 * width)
      last = last < finish ? last : finish

      isort_block!(left, right, block)
      isort_block!(right, last, block)

      left += 2 * width
    end

    width = 7
    while width < @total
      left = @start
      while left < finish
        right = left + width
        right = right < finish ? right : finish
        last = left + (2 * width)
        last = last < finish ? last : finish

        bottom_up_merge_block(left, right, last, block)
        left += 2 * width
      end

      @tuple, @scratch = @scratch, @tuple
      width *= 2
    end

    @scratch = nil
    self
  end
  private :mergesort_block!

  def bottom_up_merge_block(left, right, last, block)
    left_index = left
    right_index = right
    i = left

    while i < last
      if left_index < right && (right_index >= last || block.call(@tuple.at(left_index), @tuple.at(right_index)) <= 0)
        @scratch[i] = @tuple.at(left_index)
        left_index += 1
      else
        @scratch[i] = @tuple.at(right_index)
        right_index += 1
      end

      i += 1
    end
  end
  private :bottom_up_merge_block

  # Insertion sort in-place between the given indexes.
  def isort!(left, right)
    i = left + 1

    tup = @tuple

    while i < right
      j = i

      while j > left
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

    while i < right
      j = i

      while j > left
        block_result = block.call(@tuple.at(j - 1), @tuple.at(j))

        if block_result.nil?
          raise ArgumentError, 'block returned nil'
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
