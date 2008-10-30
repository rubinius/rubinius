# depends on: class.rb enumerable.rb tuple.rb kernel.rb

##
# Arrays are ordered, integer-indexed collections of any object.  Array
# indexing starts at 0, as in C or Java.  A negative index is assumed to be
# relative to the end of the array---that is, an index of -1 indicates the
# last element of the array, -2 is the next to last element in the array, and
# so on.
#
# Arrays can be created with the <tt>[]</tt> syntax, or via <tt>Array.new</tt>.

class Array
  def total=(n) ; @total = n ; end
  def tuple=(t) ; @tuple = t ; end
  def start=(s) ; @start = s ; end

  include Enumerable

  # The flow control for many of these methods is
  # pretty evil due to how MRI works. There is
  # also a lot of duplication of code due to very
  # subtle processing differences and, in some
  # cases, to avoid mutual dependency. Apologies.


  # Returns a new Array populated with the given objects
  def self.[](*args)
    new args
  end

  def self.allocate
    ary = super()
    ary.start = 0
    ary.total = 0
    ary.tuple = Tuple.new 8
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
  def initialize(*args)
    raise ArgumentError, "Wrong number of arguments, #{args.size} for 2" if args.size > 2

    unless args.empty?
      if args.size == 1 and (args.first.__kind_of__ Array or args.first.respond_to? :to_ary)
        ary = Type.coerce_to args.first, Array, :to_ary

        tuple = Tuple.new(ary.size + 10)
        @total = ary.size
        tuple.copy_from ary.tuple, ary.start, 0
        @tuple = tuple
      else
        count = Type.coerce_to args.first, Fixnum, :to_int
        raise ArgumentError, "Size must be positive" if count < 0
        obj = args[1]

        @tuple = Tuple.new(count + 10)
        @total = count

        if block_given?
          i = 0
          while i < count
            @tuple.put i, yield(i)
            i += 1
          end          
        else
          i = 0
          while i < count
            @tuple.put i, obj
            i += 1
          end
        end
      end
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
  def [](one, two = nil)
    Ruby.primitive :array_aref

    # Normalise the argument variants
    start, finish, count, simple, is_range = nil, nil, nil, false, false

    if one.kind_of? Range
      is_range = true
      start, finish = one.begin, one.end
    elsif two
      start, count = one, Type.coerce_to(two, Fixnum, :to_int)
      return nil if count < 0       # No need to go further
    else
      start, finish, simple = one, one, true
    end

    # Convert negative indices
    start = Type.coerce_to start, Fixnum, :to_int
    start += @total if start < 0

    if simple
      return nil if start < 0 or start >= @total
      return @tuple.at(@start + start)

    # ONE past end only, MRI compat
    elsif start == @total
      return self.class.new

    elsif start < 0 or start >= @total
      return nil
    end


    finish = Type.coerce_to finish, Fixnum, :to_int if finish
    finish = (start + count - 1) if count    # For non-ranges

    finish += @total if finish < 0

    finish -= 1 if is_range and one.exclude_end?

    # Going past the end is ignored (sort of)
    finish = (@total - 1) if finish >= @total

    if finish < 0
      return self.class.new if is_range
      return nil
    end

    return self.class.new if finish < start or count == 0

    out = self.class.new
    i = start
    while(i <= finish)
      out << at(i)
      i += 1
    end
    out
  end

  alias_method :slice, :[]

  def []=(idx, ent, *args)
    Ruby.primitive :array_aset

    cnt = nil
    if args.size != 0
      cnt = ent.to_int
      ent = args[0]             # 2nd arg (cnt) is the optional one!
    end

    # Normalise Ranges
    if idx.is_a?(Range)
      if cnt
        raise ArgumentError, "Second argument invalid with a range"
      end

      unless idx.first.respond_to?(:to_int)
        raise TypeError, "can't convert #{idx.first.class} into Integer"
      end

      unless idx.last.respond_to?(:to_int)
        raise TypeError, "can't convert #{idx.last.class} into Integer"
      end

      lst = idx.last.to_int
      if lst < 0
        lst += @total
      end
      lst += 1 unless idx.exclude_end?

      idx = idx.first.to_int
      if idx < 0
        idx += @total
        raise RangeError if idx < 0
      end

      # m..n, m > n allowed
      lst = idx if idx > lst

      cnt = lst - idx
    end

    idx = idx.to_int

    if idx < 0
      idx += @total
      raise IndexError.new("Index #{idx -= @total} out of bounds") if idx < 0
    end

    if cnt
      # count < 0 not allowed
      raise IndexError.new("Negative length #{cnt}") if cnt < 0

      cnt = @total - idx if cnt > @total - idx # MRI seems to be forgiving here!

      if ent.nil?
        replacement = []
      elsif ent.is_a?(Array)
        replacement = ent.dup
      elsif ent.respond_to?(:to_ary)
        replacement = ent.to_ary
      else
        replacement = [ent]
      end

      if replacement.size > cnt
        newtotal = @total + replacement.size - cnt
        if newtotal > @tuple.fields - @start
          nt = Tuple.new(newtotal + 10)
          nt.copy_from @tuple, @start, 0 # FIXME: double copy of right part
          @start = 0
          @tuple = nt
        end                     # this should be an else
        f = @total
        t = newtotal
        while f > idx + cnt
          t -= 1
          f -= 1
          @tuple.put(@start+t, @tuple.at(@start+f))
        end
        @total = newtotal
      end
      replacement.each_with_index { |el, i|
        @tuple.put(@start+idx+i, el)
      }

      if replacement.size < cnt
        f = @start + idx + cnt
        t = @start + idx + replacement.size

        # shift fields to the left
        while f < @total
          @tuple.put(t, @tuple.at(f))
          t += 1
          f += 1
        end

        # unset any extraneous fields
        while t < @tuple.fields
          @tuple.put(t, nil)
          t += 1
        end

        @total -= (cnt - replacement.size)
      end

      return ent
    end

    nt = @start + idx + 1
    reallocate(nt) if @tuple.size < nt

    @tuple.put @start + idx, ent
    if idx >= @total - 1
      @total = idx + 1
    end
    return ent
  end

  # Appends the object to the end of the Array.
  # Returns self so several appends can be chained.
  def <<(obj)
    nt = @start + @total + 1
    reallocate nt if @tuple.size < nt
    @tuple.put @start + @total, obj
    @total += 1
    self
  end

  # Creates a new Array containing only elements common to
  # both Arrays, without duplicates. Also known as a 'set
  # intersection'
  def &(other)
    other = Type.coerce_to other, Array, :to_ary

    out, set_include = [], {}

    other.each { |x| set_include[x] = [true, x] }
    each { |x|
      if set_include[x] and set_include[x].last.eql?(x)
        out << x
        set_include[x] = false
      end
    }

    out
  end

  # Creates a new Array by combining the two Arrays' items,
  # without duplicates. Also known as a 'set union.'
  def |(other)
    other = Type.coerce_to other, Array, :to_ary

    out, exclude = [], {}

    (self + other).each { |x|
      unless exclude[x]
        out << x
        exclude[x] = true
      end
    }

    out
  end

  # Repetition operator when supplied a #to_int argument:
  # returns a new Array as a concatenation of the given number
  # of the original Arrays. With an argument that responds to
  # #to_str, functions exactly like #join instead.
  def *(val)
    if val.respond_to? :to_str
      return join(val)

    else
      # Aaargh stupid MRI's stupid specific stupid error stupid types stupid
      val = Type.coerce_to val, Fixnum, :to_int

      raise ArgumentError, "Count cannot be negative" if val < 0

      out = self.class.new
      val.times { out.push(*self) }
      out
    end
  end

  # Create a concatenation of the two Arrays.
  def +(other)
    other = Type.coerce_to other, Array, :to_ary
    Array.new(self).concat(other)
  end

  # Creates a new Array that contains the items of the original
  # Array that do not appear in the other Array, effectively
  # 'deducting' those items. The matching method is Hash-based.
  def -(other)
    other = Type.coerce_to other, Array, :to_ary
    out, exclude = [], {}

    other.each { |x| exclude[x] = true }
    each { |x| out << x unless exclude[x] }

    out
  end

  # Compares the two Arrays and returns -1, 0 or 1 depending
  # on whether the first one is 'smaller', 'equal' or 'greater'
  # in relation to the second. Two Arrays are equal only if all
  # their elements are 0 using first_e <=> second_e and their
  # lengths are the same. The element comparison is the primary
  # and length is only checked if the former results in 0's.
  def <=>(other)
    other = Type.coerce_to other, Array, :to_ary

    size.times { |i|
      return 1 unless other.size > i

      diff = at(i) <=> other.at(i)
      return diff if diff != 0
    }

    return 1 if size > other.size
    return -1 if size < other.size
    0
  end

  # The two Arrays are considered equal only if their
  # lengths are the same and each of their elements
  # are equal according to first_e == second_e . Both
  # Array subclasses and to_ary objects are accepted.
  def ==(other)
    unless other.kind_of? Array
      return false unless other.respond_to? :to_ary
      other = other.to_ary
    end

    return false unless size == other.size

    i = 0
    while(i < size)
      return false unless @tuple.at(@start + i) == other.at(i)
      i += 1
    end

    true
  end

  # Assumes the Array contains other Arrays and searches through
  # it comparing the given object with the first element of each
  # contained Array using elem == obj. Returns the first contained
  # Array that matches (the first 'associated' Array) or nil.
  def assoc(obj)
    # FIX: use break when it works again
    found, res = nil, nil

    each { |elem|
      if found.nil? and elem.kind_of? Array and elem.first == obj
        found, res = true, elem
      end
    }

    res
  end

  # Returns the element at the given index. If the
  # index is negative, counts from the end of the
  # Array. If the index is out of range, nil is
  # returned. Slightly faster than +Array#[]+
  def at(idx)
    Ruby.primitive :array_aref
    idx = Type.coerce_to idx, Fixnum, :to_int
    idx += @total if idx < 0

    return nil if idx < 0 or idx >= @total

    @tuple.at @start + idx
  end

  # Removes all elements in the Array and leaves it empty
  def clear()
    @tuple = Tuple.new(1)
    @total = 0
    @start = 0
    self
  end

  # Returns a copy of self with all nil elements removed
  def compact()
    dup.compact! || self
  end

  # Removes all nil elements from self, returns nil if no changes
  # TODO: Needs improvement
  def compact!()
    i = @start
    tot = @start + @total

    # Low-level because pretty much anything else breaks everything
    while i < tot
      if @tuple.at(i).nil?
        j = i
        i += 1

        while i < tot
          if @tuple.at(i) != nil
            @tuple.put j, @tuple.at(i)
            j += 1
          end

          i += 1
        end

        # OK to leave tuple size larger?
        @total = j - @start
        return self
      end

      i += 1
    end

    nil
  end

  # Appends the elements in the other Array to self
  def concat(other)
    ary = Type.coerce_to(other, Array, :to_ary)
    size = @total + ary.size
    tuple = Tuple.new size
    tuple.copy_from @tuple, @start, 0 if @total > 0
    tuple.copy_from ary.tuple, ary.start, @total
    @tuple = tuple
    @start = 0
    @total = size
    self
  end

  # Stupid subtle differences prevent proper reuse in these three

  # Removes all elements from self that are #== to the given object.
  # If the object does not appear at all, nil is returned unless a
  # block is provided in which case the value of running it is
  # returned instead.
  def delete(obj)
    i = @start
    tot = @start + @total

    # Leaves the tuple to the original size still
    while i < tot
      if @tuple.at(i) == obj
        j = i
        i += 1

        while i < tot
          if @tuple.at(i) != obj
            @tuple.put(j, @tuple.at(i))
            j += 1
          end

          i += 1
        end

        @total = j - @start
        return obj
      end

      i += 1
    end

    yield if block_given?
  end

  # Deletes the element at the given index and returns
  # the deleted element or nil if the index is out of
  # range. Negative indices count backwards from end.
  def delete_at(idx)
    idx = Type.coerce_to idx, Fixnum, :to_int

    # Flip to positive and weed out out of bounds
    idx += @total if idx < 0
    return nil if idx < 0 or idx >= @total

    # Grab the object and adjust the indices for the rest
    obj = @tuple.at(@start + idx)

    idx.upto(@total - 2) { |i| @tuple.put(@start + i, @tuple.at(@start + i + 1)) }
    @tuple.put(@start + @total - 1, nil)

    @total -= 1
    obj
  end

  # Deletes every element from self for which block evaluates to true
  def delete_if()
    i = @start
    tot = @total + @start

    # Leaves the tuple to the original size still
    while i < tot
      if yield @tuple.at(i)
        j = i
        i += 1

        while i < tot
          unless yield @tuple.at(i)
            @tuple.put(j, @tuple.at(i))
            j += 1
          end

          i += 1
        end

        @total = j - @start
        return self
      end

      i += 1
    end

    return self
  end

  # Passes each element in the Array to the given block
  # and returns self.  We re-evaluate @total each time
  # through the loop in case the array has changed.
  def each()
    i = 0
    while i < @total
      yield at(i)
      i += 1
    end
    self
  end

  # Passes each index of the Array to the given block
  # and returns self.  We re-evaluate @total each time
  # through the loop in case the array has changed.
  def each_index()
    i = 0
    while i < @total
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

    each_with_index { |o, i| return false unless o.eql?(other[i]) }

    true
  end

  # True if Array has no elements.
  def empty?()
    @total == 0
  end

  # Attempts to return the element at the given index. By default
  # an IndexError is raised if the element is out of bounds. The
  # user may supply either a default value or a block that takes
  # the index object instead.
  def fetch(idx, *rest)
    raise ArgumentError, "Expected 1-2, got #{1 + rest.length}" if rest.length > 1
    warn 'Block supercedes default object' if !rest.empty? && block_given?

    idx, orig = Type.coerce_to(idx, Fixnum, :to_int), idx
    idx += @total if idx < 0

    if idx < 0 || idx >= @total
      return yield(orig) if block_given?
      return rest.at(0) unless rest.empty?

      raise IndexError, "Index #{idx} out of array" if rest.empty?
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
  # array.fill {|index| block }                    -> array
  # array.fill(start [, length]) {|index| block }  -> array
  # array.fill(range) {|index| block }             -> array
  #
  def fill(*args)
    raise ArgumentError, "Wrong number of arguments" if block_given? and args.size > 2
    raise ArgumentError, "Wrong number of arguments" if args.size > 3

    # Normalise arguments
    start, finish, obj = 0, (@total - 1), nil

    obj = args.shift unless block_given?
    one, two = args.at(0), args.at(1)

    if one.kind_of? Range
      raise TypeError, "Length invalid with range" if args.size > 1   # WTF, MRI, TypeError?

      start, finish = Type.coerce_to(one.begin, Fixnum, :to_int), Type.coerce_to(one.end, Fixnum, :to_int)

      start += @total if start < 0
      finish += @total if finish < 0

      if one.exclude_end?
        return self if start == finish
        finish -= 1
      end

      raise RangeError, "#{one.inspect} out of range" if start < 0
      return self if finish < 0           # Nothing to modify

    else
      if one
        start = Type.coerce_to one, Fixnum, :to_int

        start += @total if start < 0
        start = 0 if start < 0            # MRI comp adjusts to 0

        if two
          finish = Type.coerce_to two, Fixnum, :to_int
          return self if finish < 1       # Nothing to modify
          raise ArgumentError, "argument too big" if finish < 0 && start < finish.abs

          finish = start + finish - 1
        end
      end
    end                                   # Argument normalisation

    # Adjust the size progressively
    unless finish < @total
      nt = finish + 1
      reallocate(nt) if @tuple.size < nt
      @total = finish + 1
    end

    if block_given?
      start.upto(finish) { |i|  @tuple.put @start + i, yield(i) }
    else
      start.upto(finish) { |i|  @tuple.put @start + i, obj }
    end

    self
  end

  # Returns the first or first n elements of the Array.
  # If no argument is given, returns nil if the item
  # is not found. If there is an argument, an empty
  # Array is returned instead.
  def first(n = nil)
    return at(0) unless n

    n = Type.coerce_to n, Fixnum, :to_int
    raise ArgumentError, "Size must be positive" if n < 0

    Array.new(self[0...n])
  end

  # Recursively flatten any contained Arrays into an one-dimensional result.
  def flatten()
    dup.flatten! || self
  end

  # Flattens self in place as #flatten. If no changes are
  # made, returns nil, otherwise self.
  def flatten!
    ret, out = nil, []

    ret = recursively_flatten(self, out)
    replace(out) if ret
    ret
  end

  # Computes a Fixnum hash code for this Array. Any two
  # Arrays with the same content will have the same hash
  # code (similar to #eql?)
  def hash()
    # IMPROVE: This is a really really poor implementation of hash for an array, but
    # it does work. It should be replaced with something much better, but I'm not sure
    # what level it belongs at.
    str = ""
    each { |item| str << item.hash.to_s }
    str.hash
  end

  # Returns true if the given obj is present in the Array.
  # Presence is determined by calling elem == obj until found.
  def include?(obj)
    i = 0
    while i < @total do
      return true if @tuple.at(@start + i) == obj
      i += 1
    end
    false
  end

  # Returns the index of the first element in the Array
  # for which elem == obj is true or nil.
  def index(obj)
    i = 0
    while i < @total do
      return i if @tuple.at(@start + i) == obj
      i += 1
    end
    nil
  end

  # Returns an Array populated with the objects at the given indices of the original.
  # Range arguments are given as nested Arrays as from #[].
  def indexes(*args)
    warn 'Array#indexes is deprecated, use Array#values_at instead'

    out = []

    args.each { |a|
      if a.kind_of? Range
        out << self[a]
      else
        out << at(Type.coerce_to(a, Fixnum, :to_int))
      end
    }

    out
  end

  alias_method :indices, :indexes

  # For a positive index, inserts the given values before
  # the element at the given index. Negative indices count
  # backwards from the end and the values are inserted
  # after them.
  def insert(idx, *items)
    return self if items.length == 0

    # Adjust the index for correct insertion
    idx = Type.coerce_to idx, Fixnum, :to_int
    idx += (@total + 1) if idx < 0    # Negatives add AFTER the element
    raise IndexError, "#{idx} out of bounds" if idx < 0

    self[idx, 0] = items   # Cheat
    self
  end

  # Produces a printable string of the Array. The string
  # is constructed by calling #inspect on all elements.
  # Descends through contained Arrays, recursive ones
  # are indicated as [...].
  def inspect()
    return "[...]" if RecursionGuard.inspecting?(self)

    out = []
    RecursionGuard.inspect(self) do
      each { |o|
        out << o.inspect
      }
    end

    "[#{out.join ', '}]"
  end

  # Generates a string from converting all elements of
  # the Array to strings, inserting a separator between
  # each. The separator defaults to $,. Detects recursive
  # Arrays.
  def join(sep = nil, method = :to_s)
    return "" if @total == 0
    sep ||= $,
    begin
      sep = sep.to_str
    rescue NoMethodError
      raise TypeError, "Cannot convert #{sep.inspect} to str"
    end

    out = ""
    i = 0
    while(i < @total)
      elem = at(i)
      out.append sep unless (i == 0)

      if elem.kind_of?(Array)
        if RecursionGuard.inspecting?(elem)
          out.append "[...]"
        else
          RecursionGuard.inspect(self) do
            out.append elem.join(sep, method)
          end
        end
      else
        # HACK use __send__
        # out << elem.__send__(method)
        out.append elem.to_s
      end
      i += 1
    end
    out 
  end

  # Returns the last element or n elements of self. If
  # the Array is empty, without a count nil is returned,
  # otherwise an empty Array. Always returns an Array.
  def last(n = nil)
    if @total < 1
      return if n.nil?
      return []
    end

    return at(-1) unless n

    n = Type.coerce_to n, Fixnum, :to_int
    return [] if n.zero?
    raise ArgumentError, "Number must be positive" if n < 0

    n = size if n > size
    Array.new self[-n..-1]
  end

  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    out = Array.new @total
    i = 0
    while i < @total
      out[i] = yield(at(i))
      i += 1
    end
    out
  end

  alias_method :collect, :map

  # Replaces each element in self with the return value
  # of passing that element to the supplied block.
  def map!
    i = 0
    while i < @total
      self[i] = yield(at(i))
      i += 1
    end
    self
  end

  alias_method :collect!, :map!

  # Returns number of non-nil elements in self, may be zero
  def nitems
    sum = 0
    each { |elem| sum += 1 unless elem.nil? }
    sum
  end

  ##
  #  call-seq:
  #     arr.pack ( aTemplateString ) -> aBinaryString
  #
  #  Packs the contents of <i>arr</i> into a binary sequence according to
  #  the directives in <i>aTemplateString</i> (see the table below)
  #  Directives ``A,'' ``a,'' and ``Z'' may be followed by a count,
  #  which gives the width of the resulting field. The remaining
  #  directives also may take a count, indicating the number of array
  #  elements to convert. If the count is an asterisk
  #  (``<code>*</code>''), all remaining array elements will be
  #  converted. Any of the directives ``<code>sSiIlL</code>'' may be
  #  followed by an underscore (``<code>_</code>'') to use the underlying
  #  platform's native size for the specified type; otherwise, they use a
  #  platform-independent size. Spaces are ignored in the template
  #  string. See also <code>String#unpack</code>.
  #
  #     a = [ "a", "b", "c" ]
  #     n = [ 65, 66, 67 ]
  #     a.pack("A3A3A3")   #=> "a  b  c  "
  #     a.pack("a3a3a3")   #=> "a\000\000b\000\000c\000\000"
  #     n.pack("ccc")      #=> "ABC"
  #
  #  Directives for +pack+.
  #
  #   Directive    Meaning
  #   ---------------------------------------------------------------
  #       @     |  Moves to absolute position
  #       A     |  ASCII string (space padded, count is width)
  #       a     |  ASCII string (null padded, count is width)
  #       B     |  Bit string (descending bit order)
  #       b     |  Bit string (ascending bit order)
  #       C     |  Unsigned char
  #       c     |  Char
  #       D, d  |  Double-precision float, native format
  #       E     |  Double-precision float, little-endian byte order
  #       e     |  Single-precision float, little-endian byte order
  #       F, f  |  Single-precision float, native format
  #       G     |  Double-precision float, network (big-endian) byte order
  #       g     |  Single-precision float, network (big-endian) byte order
  #       H     |  Hex string (high nibble first)
  #       h     |  Hex string (low nibble first)
  #       I     |  Unsigned integer
  #       i     |  Integer
  #       L     |  Unsigned long
  #       l     |  Long
  #       M     |  Quoted printable, MIME encoding (see RFC2045)
  #       m     |  Base64 encoded string
  #       N     |  Long, network (big-endian) byte order
  #       n     |  Short, network (big-endian) byte-order
  #       P     |  Pointer to a structure (fixed-length string)
  #       p     |  Pointer to a null-terminated string
  #       Q, q  |  64-bit number
  #       S     |  Unsigned short
  #       s     |  Short
  #       U     |  UTF-8
  #       u     |  UU-encoded string
  #       V     |  Long, little-endian byte order
  #       v     |  Short, little-endian byte order
  #       w     |  BER-compressed integer\fnm
  #       X     |  Back up a byte
  #       x     |  Null byte
  #       Z     |  Same as ``a'', except that null is added with *

  def pack schema
    # The schema is an array of arrays like [["A", "6"], ["u", "*"],
    # ["X", ""]]. It represents the parsed form of "A6u*X".  Remove
    # strings in the schema between # and \n
    schema = schema.gsub(/#.*/, '')
    schema = schema.scan(/([^\s\d\*][\d\*]*)/).flatten.map {|x|
      x.match(/([^\s\d\*])([\d\*]*)/)[1..-1]
    }

    ret = ""
    arr_idx = 0

    schema.each do |kind, t|
      # p :iter => [kind, t]
      item = self[arr_idx]
      t = nil if t.empty?

      # MRI nil compatibilty for string functions
      item = "" if item.nil? && kind =~ /[aAZbBhH]/

      # if there's no item, that means there's more schema items than
      # array items, so throw an error. All actions that DON'T
      # increment arr_idx must occur before this test.
      raise ArgumentError, "too few array elements" if
        arr_idx >= self.length and kind !~ /x/i

      case kind # TODO: switch kind to ints
      when 'X' then
        size = (t || 1).to_i
        raise ArgumentError, "you're backing up too far" if size > ret.size
        ret[-size..-1] = '' if size > 0
      when 'x' then
        size = (t || 1).to_i
        ret << "\000" * size
      when 'N' then
        parts = []
        4.times do                          # TODO: const?
          parts << (item % 256).chr
          item >>= 8
        end
        ret << parts.reverse.join
        arr_idx += 1
        item = nil
        next # HACK
      when 'V' then
        parts = []
        4.times do                          # TODO: const?
          parts << (item % 256).chr
          item >>= 8
        end
        ret << parts.join
        arr_idx += 1
      when 'v' then
        parts = []
        2.times do
          parts << (item % 256).chr
          item >>= 8
        end
        ret << parts.join
        arr_idx += 1
      when 'a', 'A', 'Z' then
        item = Type.coerce_to(item, String, :to_str)
        size = case t
               when nil
                 1
               when '*' then
                 item.size + (kind == "Z" ? 1 : 0)
               else
                 t.to_i
               end

        padsize = size - item.size
        filler  = kind == "A" ? " " : "\0"

        ret << item.split(//).first(size).join
        ret << filler * padsize if padsize > 0

        arr_idx += 1
      when 'b', 'B' then
        item = Type.coerce_to(item, String, :to_str)
        byte = 0
        lsb  = (kind == "b")
        size = case t
               when nil
                 1
               when '*' then
                 item.size
               else
                 t.to_i
               end

        bits = item.split(//).map { |c| c[0] & 01 }
        min = [size, item.size].min

        bits.first(min).each_with_index do |bit, i| # TODO: this can be cleaner
          i &= 07

          byte |= bit << (lsb ? i : 07 - i)

          if i == 07 then
            ret << byte.chr
            byte = 0
          end
        end

        # always output an incomplete byte
        if ((size & 07) != 0 || min != size) && item.size > 0 then
          ret << byte.chr
        end

        # Emulate the weird MRI spec for every 2 chars over output a \000 # FIX
        (item.length).step(size-1, 2) { |i| ret << 0 } if size > item.length

        arr_idx += 1
      when 'c', 'C' then
        size = case t
               when nil
                 1
               when '*' then
                 self.size # TODO: - arr_idx?
               else
                 t.to_i
               end

        # FIX: uhh... size is the same as length. just tests that arr_idx == 0
        raise ArgumentError, "too few array elements" if
          arr_idx + size > self.length

        sub = self[arr_idx...arr_idx+size]
        sub.map! { |o| (Type.coerce_to(o, Integer, :to_int) & 0xff).chr }
        ret << sub.join

        arr_idx += size
      when 'M' then
        # for some reason MRI responds to to_s here
        item = Type.coerce_to(item, String, :to_s)
        ret << item.scan(/.{1,73}/m).map { |line| # 75 chars per line incl =\n
          line.gsub(/[^ -<>-~\t\n]/) { |m| "=%02X" % m[0] } + "=\n"
        }.join
        arr_idx += 1
      when 'm' then # REFACTOR: merge with u
        item = Type.coerce_to(item, String, :to_str)

        ret << item.scan(/.{1,45}/m).map { |line|
          encoded = line.scan(/(.)(.?)(.?)/m).map { |a,b,c|
            a = a[0]
            b = b[0] || 0
            c = c[0] || 0

            [BASE_64_B2A[( a >> 2                    ) & 077],
             BASE_64_B2A[((a << 4) | ((b >> 4) & 017)) & 077],
             BASE_64_B2A[((b << 2) | ((c >> 6) & 003)) & 077],
             BASE_64_B2A[( c                         ) & 077]]
          }

          "#{encoded.flatten.join}\n"
        }.join.sub(/(A{1,2})\n\Z/) { "#{'=' * $1.size}\n" }

        arr_idx += 1
      when 'w' then
        item = Type.coerce_to(item, Integer, :to_i)
        raise ArgumentError, "can't compress negative numbers" if item < 0

        ret << (item & 0x7f)
        while (item >>= 7) > 0 do
          ret << ((item & 0x7f) | 0x80)
        end

        ret.reverse! # FIX - breaks anything following BER?
        arr_idx += 1
      when 'u' then # REFACTOR: merge with m
        item = Type.coerce_to(item, String, :to_str)

        # http://www.opengroup.org/onlinepubs/009695399/utilities/uuencode.html
        ret << item.scan(/.{1,45}/m).map { |line|
          encoded = line.scan(/(.)(.?)(.?)/m).map { |a,b,c|
            a = a[0]
            b = b[0] || 0
            c = c[0] || 0

            [(?\s + (( a >> 2                    ) & 077)).chr,
             (?\s + (((a << 4) | ((b >> 4) & 017)) & 077)).chr,
             (?\s + (((b << 2) | ((c >> 6) & 003)) & 077)).chr,
             (?\s + (( c                         ) & 077)).chr]
          }.flatten

          "#{(line.size + ?\s).chr}#{encoded.join}\n"
        }.join.gsub(/ /, '`')
        arr_idx += 1
      when 'i', 's', 'l', 'n', 'I', 'S', 'L' then
        size = case t
               when nil
                 1
               when '*' then
                 self.size
               else
                 t.to_i
               end

        native        = t && t == '_'
        unsigned      = (kind =~ /I|S|L/)
        little_endian = kind !~ /n/i && endian?(:little)

        raise "unsupported - fix me" if native

        unless native then
          bytes = case kind
                  when /n/i then 2
                  when /s/i then 2
                  when /i/i then 4
                  when /l/i then 4
                  end
        end

        size.times do |i|
          item = Type.coerce_to(self[arr_idx], Integer, :to_i)

          # MRI seems only only raise RangeError at 2**32 and above, even shorts
          raise RangeError, "bignum too big to convert into 'unsigned long'" if
            item.abs >= 2**32 # FIX: const

            ret << if little_endian then
                     item += 2 ** (8 * bytes) if item < 0
                     (0...bytes).map { |b| ((item >> (b * 8)) & 0xFF).chr }
                   else # ugly
                     (0...bytes).map {n=(item % 256).chr;item /= 256; n}.reverse
                   end.join
          arr_idx += 1
        end
      when 'H', 'h' then
        size = if t.nil?
                 0
               elsif t == "*"
                 item.length
               else
                 t.to_i
               end
        str = item.scan(/..?/).first(size)

        ret << if kind == "h" then
                 str.map { |b| b.reverse.hex.chr }.join
               else
                 str.map { |b| b.        hex.chr }.join
               end

        arr_idx += 1
      when 'U' then
        count = if t.nil? then
                  1
                elsif t == "*"
                  self.size - arr_idx
                else
                  t.to_i
                end

        raise ArgumentError, "too few array elements" if
          arr_idx + count > self.length

        count.times do
          item = Type.coerce_to(self[arr_idx], Integer, :to_i)

          raise RangeError, "pack(U): value out of range" if item < 0

          bytes = 0
          f = [2 ** 7, 2 ** 11, 2 ** 16, 2 ** 21, 2 ** 26, 2 ** 31].find { |n|
            bytes += 1
            item < n
          }

          raise RangeError, "pack(U): value out of range" if f.nil?

          if bytes == 1 then
            ret << item
            bytes = 0
          end

          i = bytes

          buf = []
          if i > 0 then
            (i-1).times do
              buf.unshift((item | 0x80) & 0xBF)
              item >>= 6
            end
            # catch the highest bits - the mask depends on the byte count
            buf.unshift(item | ((0x3F00 >> bytes)) & 0xFC)
          end

          ret << buf.map { |n| n.chr }.join

          arr_idx += 1
        end
      else
        raise ArgumentError, "Unknown kind #{kind}"
      end
    end

    return ret
  end

  # Removes and returns the last element from the Array.
  def pop()
    return nil if empty?

    # TODO Reduce tuple if there are a lot of free slots

    elem = at(-1)
    @total -= 1
    elem
  end

  # Rubinius-only, better inspect representation of the Array
  def indented_inspect(indent = 0)
    # Here there be dragons. In fact, there is one jusAAAAAAAARGH
    str = "["

    sub = false
    i = 0
    lst = size - 1
    while i < size
      element = self[i]
      if Array === element
        estr = element.indented_inspect(indent + 2)
        if str.size > 30 or estr.size > 30
          if estr[0] != ?\s
            estr = "#{' ' * (indent + 2)}#{estr}"
          end

          str << "\n#{estr}"
          sub = true
        else
          str << estr
        end
      else
        str << element.inspect
      end

      str << ", " unless i == lst
      i += 1
    end

    if sub
      str << "\n#{' ' * indent}]"
    else
      str << "]"
    end

    if sub
      return "#{' ' * indent}#{str}"
    end

    return str
  end

  # Appends the given object(s) to the Array and returns
  # the modified self.
  def push(*args)
    args.each { |ent| self << ent }
    self
  end

  # Searches through contained Arrays within the Array,
  # comparing obj with the second element of each using
  # elem == obj. Returns the first matching Array, nil
  # on failure. See also Array#assoc.
  def rassoc(obj)
    # FIX: Use break when it works
    found, res = nil, nil

    each { |elem|
      if found.nil? and elem.kind_of? Array and elem.at(1) == obj
        res, found = elem, true
      end
    }

    res
  end

  # Returns a new Array by removing items from self for
  # which block is true. An Array is also returned when
  # invoked on subclasses. See #reject!
  def reject(&block)
    Array.new(self).reject!(&block) || self
  end

  # Equivalent to #delete_if except that returns nil if
  # no changes were made.
  def reject!(&block)
    was = length
    self.delete_if(&block)

    self if was != length     # Too clever?
  end

  # Replaces contents of self with contents of other,
  # adjusting size as needed.
  def replace(other)
    other = Type.coerce_to other, Array, :to_ary

    @tuple = other.tuple.dup
    @total = other.total
    @start = other.start
    self
  end

  # Returns a new Array or subclass populated from self
  # but in reverse order.
  def reverse()
    dup.reverse!
  end

  # Reverses the order of elements in self. Returns self
  # even if no changes are made
  def reverse!
    return self unless @total > 1

    tuple = Tuple.new @total
    i = 0
    j = @start + @total - 1
    while(j >= @start) do
      tuple.put i, @tuple.at(j)
      i += 1
      j -= 1      
    end

    @tuple = tuple
    @start = 0

    self
  end

  # Goes through the Array back to front and yields
  # each element to the supplied block. Returns self.
  def reverse_each()
    i = @total - 1
    while i >= 0 do
      yield(at(i))
      i -= 1
    end
    self
  end

  # Returns the index of the last element in the Array
  # for which elem == obj is true.
  def rindex(obj)
    i = @total - 1
    while i >= 0 do
      return i if at(i) == obj
      i -= 1
    end
    nil
  end

  # Removes and returns the first element in the
  # Array or nil if empty. All other elements are
  # moved down one index.
  def shift()
    return nil if @total == 0

    obj = @tuple.at(@start)
    @start += 1
    @total -= 1

    obj
  end

  # Deletes the element(s) given by an index (optionally with a length)
  # or by a range. Returns the deleted object, subarray, or nil if the
  # index is out of range. Equivalent to:
  def slice!(*args)
    out = self[*args]
    if !(Range === args[0])
      # make sure that negative values are not passed through to the
      # []= assignment
      args[0] = Type.coerce_to args[0], Integer, :to_int
      args[0] = args[0] + self.length if args[0] < 0
      # This is to match the MRI behaviour of not extending the array
      # with nil when specifying an index greater than the length
      # of the array.
      if args.size == 1
        return out unless args[0] >= 0 && args[0] < self.length
        args << 1
      end
    end
    self[*args] = []
    out
  end

  # Returns a new Array created by sorting elements in self. Comparisons
  # to sort are either done using <=> or a block may be provided. The
  # block must take two parameters and return -1, 0, 1 depending on
  # whether the first parameter is smaller, equal or larger than the
  # second parameter.
  def sort(&block)
    dup.sort!(&block)
  end

  # Sorts this Array in-place. See #sort.
  def sort!(&block)
    return self unless @total > 1

    if (@total - @start) < 6
      if block
        isort_block! @start, (@total - 1), block
      else
        isort! @start, (@total - 1)
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

  # Returns self except on subclasses which are converted
  # or 'upcast' to Arrays.
  def to_a()
    if self.class == Array
      self
    else
      Array.new(self[0..-1])
    end
  end

  # Returns self
  def to_ary()
    self
  end

  # Produces a string by joining all elements without a
  # separator. See #join
  def to_s()
    self.join
  end

  # Treats all elements as being Arrays of equal lengths and
  # transposes their rows and columns so that the first contained
  # Array in the result includes the first elements of all the
  # Arrays and so on.
  def transpose()
    return [] if empty?

    out, max = [], nil

    each { |ary|
      ary = Type.coerce_to ary, Array, :to_ary
      max ||= ary.size

      # Catches too-large as well as too-small (for which #fetch would suffice)
      raise IndexError, "All arrays must be same length" if ary.size != max

      ary.size.times { |i| (out[i] ||= []) << ary.at(i) }
    }

    out
  end

  # Returns a new Array by removing duplicate entries
  # from self. Equality is determined by using a Hash
  def uniq()
    seen, out = {}, self.class.new

    each { |elem|
      out << elem unless seen[elem]
      seen[elem] = true
    }

    out
  end

  # Removes duplicates from the Array in place as #uniq
  def uniq!()
    ary = uniq
    replace(ary) if size != ary.size
  end

#  # Inserts the element to the front of the Array and
#  # moves the other elements up by one index.
#  def unshift(*val)
#    raise TypeError, "Array is frozen" if frozen?
#    return self if val.empty?
#
#    self[0, 0] = val
#    self
#  end

  # Returns a new Array populated from the elements in
  # self corresponding to the given selector(s.) The
  # arguments may be one or more integer indices or
  # Ranges.
  def values_at(*args)
    out = []

    args.each { |elem|
      # Cannot use #[] because of subtly different errors
      if elem.kind_of? Range
        finish = elem.last
        start = elem.first

        start, finish = Type.coerce_to(start, Fixnum, :to_int), Type.coerce_to(finish, Fixnum, :to_int)

        start += @total if start < 0
        next if start < 0

        finish += @total if finish < 0
        finish -= 1 if elem.exclude_end?
        finish = @total unless finish < @total

        next if finish < start

        start.upto(finish) { |i| out << at(i) }

      else
        i = Type.coerce_to elem, Fixnum, :to_int
        out << at(i)
      end
    }

    out
  end

  # Interleaves all given :to_ary's so that the n-th element of each
  # Array is inserted into the n-th subarray of the returned Array.
  # If a block is provided, then each subarray is passed to it
  # instead. The maximum number of subarrays and therefore elements
  # used is the size of self. Missing indices are filled in with
  # nils and any elements past self.size in the other Arrays are
  # ignored.
  def zip(*others)
    out = Array.new(size) { [] }
    others = others.map { |ary| ary.to_a }

    size.times do |i|
      slot = out.at(i)
      slot << @tuple.at(@start + i)
      others.each { |ary| slot << ary.at(i) }
    end

    if block_given?
      out.each { |ary| yield ary }
      return nil
    end

    out
  end

  def size
    @total
  end

  def length
    @total
  end

  def unshift(*values)
    while values.size > 0 && @start > 0
      @start -= 1
      @total += 1
      @tuple.put @start, values.pop
    end

    @tuple = @tuple.shifted(values.size)

    values.each_with_index do |val, i|
      @tuple.put i, val
    end

    @total += values.size
    self
  end

  # Exactly the same as #replace but private
  def initialize_copy(other)
    replace other
  end

  private :initialize_copy

  # Reallocates the internal Tuple to accommodate at least given size
  def reallocate(at_least)
    return if at_least < @tuple.size

    new_size = @tuple.size * 2

    if new_size < at_least
      new_size = at_least
    end

    tuple = Tuple.new(new_size)

    tuple.copy_from @tuple, @start, 0     # Swap over old data

    @tuple = tuple
    @start = 0
  end

  private :reallocate

  # Helper to recurse through flattening since the method
  # is not allowed to recurse itself. Detects recursive structures.
  def recursively_flatten(array, out, recursive_placeholder = Undefined)
    if RecursionGuard.inspecting?(array)
      if recursive_placeholder.equal? Undefined
        raise ArgumentError, "tried to flatten recursive array"
      else
        out << recursive_placeholder
        return nil
      end
    end

    ret = nil
    array.each { |o|
      if o.respond_to? :to_ary
        RecursionGuard.inspect(array) do
          ary = Type.coerce_to o, Array, :to_ary
          recursively_flatten(ary, out, recursive_placeholder)
          ret = self
        end
      else
        out << o
      end
    }

    ret
  end

  private :recursively_flatten

  def remove_outer_arrays(array=self)
    if RecursionGuard.inspecting?(array)
      array
    elsif array.size == 1 && array.first.kind_of?(Array)
      new_array = nil
      RecursionGuard.inspect(array) do
        new_array = remove_outer_arrays(array.first)
      end
      new_array
    else
      array
    end
  end

  private :remove_outer_arrays

  ISORT_THRESHOLD   = 7
  MEDIAN_THRESHOLD  = 11

  # In-place non-recursive sort between the given indexes.
  def qsort!()
    # Stack stores the indexes that still need sorting
    stack = []
    left_end, right_end = @start, (@total - 1)

    # We are either processing a 'new' partition or one that
    # was saved to stack earlier.
    while true
      left, right = left_end, (right_end - 1)   # Leave room for pivot
      eqls_left, eqls_right = (left_end - 1), right_end

      # Choose pivot from median
      # CAUTION: This is NOT the same as #qsort_block!
      middle = left_end + ((right_end - left_end) / 2)
      low, mid, hi = @tuple.at(left_end), @tuple.at(middle), @tuple.at(right_end)

      segment_size = right_end - left_end

      # "Heuristic" to avoid problems with reverse-sorted
      if segment_size > 1000 and (low <=> mid) == 1 and (mid <=> hi) == 1
        semi_left = @tuple.at(left_end + ((middle - left_end) / 2))
        semi_right = @tuple.at(middle + ((right_end - middle) / 2))

        if (low <=> semi_left) == 1 and (semi_left <=> mid) == 1 and
           (mid <=> semi_right) == 1 and (semi_right <=> hi) == 1

          r = segment_size / 4
          while r > 0
            @tuple.swap(rand(segment_size), rand(segment_size))
            r -= 1
          end

          middle += (right_end - middle) / 2
        end
      end

      # These can be reordered which may help with sorting randomly
      # distributed elements at the cost of presorted. Be sure to
      # mark down the correct order though..
      @tuple.swap(left_end, right_end)  if (hi <=> low) < 0
      @tuple.swap(left_end, middle)     if (mid <=> low) < 0
      @tuple.swap(middle, right_end)    if (hi <=> mid) < 0

      pivot = @tuple.at(middle)
      @tuple.swap(right_end, middle)  # Known position to help partition three ways

      # Partition
      while true
        while left < right_end
          case @tuple.at(left) <=> pivot
          when -1
            left += 1
          when 0
            @tuple.swap(left, (eqls_left += 1))
            left += 1
          else
            break
          end
        end

        while right > left_end
          case @tuple.at(right) <=> pivot
          when 1
            right -= 1
          when 0
            @tuple.swap(right, (eqls_right -= 1))
            right -= 1
          else
            break
          end
        end

        break if left >= right
        @tuple.swap(left, right)
      end

      # Move pivot back to the middle
      @tuple.swap(left, right_end)
      left, right = (left - 1), (left + 1)

      # Move the stashed == pivot elements back to the middle
      while eqls_left >= left_end
        @tuple.swap(eqls_left, left)
        left -= 1
        eqls_left -= 1
      end

      unless right >= right_end
        while eqls_right < right_end and right < right_end
          @tuple.swap(eqls_right, right)
          right += 1
          eqls_right += 1
        end
      end

      # Continue processing the now smaller partitions or if
      # done with this segment, restore a stored one from the
      # stack until nothing remains either way.
      left_size, right_size = (left - left_end), (right_end - right)

      # Insertion sort is faster at anywhere below 7-9 elements
      if left_size < ISORT_THRESHOLD
        isort! left_end, left

        # We can restore next saved if both of these are getting sorted
        if right_size < ISORT_THRESHOLD
          isort! right, right_end
          break if stack.empty?       # Completely done, no stored ones left either
          left_end, right_end = stack.pop
        else
          left_end = right
        end

      elsif right_size < ISORT_THRESHOLD
        isort! right, right_end
        right_end = left

      # Save whichever is the larger partition and do the smaller first
      else
        if left_size > right_size
          stack.push [left_end, left]
          left_end = right
        else
          stack.push [right, right_end]
          right_end = left
        end
      end
    end
  end

  # In-place non-recursive sort between the given indexes using a block.
  def qsort_block!(block)
    # Stack stores the indexes that still need sorting
    stack = []
    left_end, right_end = @start, (@total - 1)

    # We are either processing a 'new' partition or one that
    # was saved to stack earlier.
    while true
      left, right = left_end, (right_end - 1)   # Leave room for pivot
      eqls_left, eqls_right = (left_end - 1), right_end

      # Choose pivot from median
      # CAUTION: This is NOT the same as #qsort!
      middle = left_end + ((right_end - left_end) / 2)
      low, mid, hi = @tuple.at(left_end), @tuple.at(middle), @tuple.at(right_end)

      segment_size = right_end - left_end

      # "Heuristic" for reverse-sorted
      if segment_size > 1000 and block.call(low, mid) == 1 and block.call(mid, hi) == 1
        semi_left = @tuple.at(left_end + ((middle - left_end) / 2))
        semi_right = @tuple.at(middle + ((right_end - middle) / 2))

        if block.call(low, semi_left) == 1 and block.call(semi_left, mid) == 1 and
           block.call(mid, semi_right) == 1 and block.call(semi_right, hi) == 1

          r = segment_size / 4
          while r > 0
            @tuple.swap(rand(segment_size), rand(segment_size))
            r -= 1
          end

          middle += (right_end - middle) / 2
        end
      end

      # These can be reordered which may help with sorting randomly
      # distributed elements at the cost of presorted. Be sure to
      # mark down the correct order though..
      @tuple.swap(left_end, right_end)  if block.call(hi, low)  < 0
      @tuple.swap(left_end, middle)     if block.call(mid, low) < 0
      @tuple.swap(middle, right_end)    if block.call(hi, mid)  < 0

      pivot = @tuple.at(middle)
      @tuple.swap(right_end, middle)  # Known position to help partition three ways

      # Partition
      while true
        while left < right_end
          case block.call(@tuple.at(left), pivot)
          when -1
            left += 1
          when 0
            @tuple.swap(left, (eqls_left += 1))
            left += 1
          else
            break
          end
        end

        while right > left_end
          case block.call(@tuple.at(right), pivot)
          when 1
            right -= 1
          when 0
            @tuple.swap(right, (eqls_right -= 1))
            right -= 1
          else
            break
          end
        end

        break if left >= right
        @tuple.swap(left, right)
      end

      # Move pivot back to the middle
      @tuple.swap(left, right_end)
      left, right = (left - 1), (left + 1)

      # Move the stashed == pivot elements back to the middle
      while eqls_left >= left_end
        @tuple.swap(eqls_left, left)
        left -= 1
        eqls_left -= 1
      end

      unless right >= right_end
        while eqls_right < right_end and right < right_end
          @tuple.swap(eqls_right, right)
          right += 1
          eqls_right += 1
        end
      end

      # Continue processing the now smaller partitions or if
      # done with this segment, restore a stored one from the
      # stack until nothing remains either way.
      left_size, right_size = (left - left_end), (right_end - right)

      # Insertion sort is faster at anywhere below 7-9 elements
      if left_size < ISORT_THRESHOLD
        isort_block! left_end, left, block

        # We can restore next saved if both of these are getting sorted
        if right_size < ISORT_THRESHOLD
          isort_block! right, right_end, block
          break if stack.empty?       # Completely done, no stored ones left either
          left_end, right_end = stack.pop
        else
          left_end = right
        end

      elsif right_size < ISORT_THRESHOLD
        isort_block! right, right_end, block
        right_end = left

      # Save whichever is the larger partition and do the smaller first
      else
        if left_size > right_size
          stack.push [left_end, left]
          left_end = right
        else
          stack.push [right, right_end]
          right_end = left
        end
      end
    end
  end

  # Insertion sort in-place between the given indexes.
  def isort!(left, right)
    i = left + 1

    while i <= right
      j = i

      while j > 0 and (@tuple.at(j - 1) <=> @tuple.at(j)) > 0
        @tuple.swap(j, (j - 1))
        j -= 1
      end

      i += 1
    end
  end

  # Insertion sort in-place between the given indexes using a block.
  def isort_block!(left, right, block)
    i = left + 1

    while i <= right
      j = i

      while j > 0 and block.call(@tuple.at(j - 1), @tuple.at(j)) > 0
        @tuple.swap(j, (j - 1))
        j -= 1
      end

      i += 1
    end
  end
  
  def __rescue_match__(exception)
    i = 0
    while i < @total
      return true if at(i) === exception
      i += 1
    end
    false
  end
  
  private :qsort
  private :isort
  private :qsort_block
  private :isort_block
end


