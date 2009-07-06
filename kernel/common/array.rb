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
    new args
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
  def initialize(size_or_array=Undefined, obj=Undefined)
    if size_or_array.equal? Undefined
      unless @total == 0
        @total = @start = 0
        @tuple = Rubinius::Tuple.new 8
      end

      return self
    end

    if obj.equal? Undefined
      obj = nil

      if size_or_array.respond_to? :to_ary
        ary = Type.coerce_to size_or_array, Array, :to_ary

        @tuple = ary.tuple.dup
        @start = ary.start
        @total = ary.size

        return self
      end
    end

    size = Type.coerce_to size_or_array, Integer, :to_int
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
  def [](arg1, arg2 = nil)
    Ruby.primitive :array_aref

    if arg1.kind_of? Range
      start_idx = Type.coerce_to arg1.begin, Fixnum, :to_int
    else
      start_idx = Type.coerce_to arg1, Fixnum, :to_int
      if arg2
        count = Type.coerce_to arg2, Fixnum, :to_int
        return nil if count < 0       # No need to go further
      else # repeat prim case after coercing with to_int
        # Convert negative indices
        start_idx += @total if start_idx < 0
        return nil if start_idx < 0 or start_idx >= @total
        return @tuple.at(@start + start_idx)
      end
    end

    # Convert negative indices
    start_idx += @total if start_idx < 0

    if start_idx < 0 or start_idx >= @total
      # ONE past end only, MRI compat
      if start_idx == @total
        return self.class.new
      else
        return nil
      end
    end

    if count
      return self.class.new if count == 0
      finish_idx = start_idx + count - 1
    else # from a range
      finish_idx = Type.coerce_to arg1.end, Fixnum, :to_int
      finish_idx += @total if finish_idx < 0
      finish_idx -= 1 if arg1.exclude_end?
    end

    if finish_idx < start_idx
      return self.class.new
    else
      # Going past the end is ignored (sort of)
      finish_idx = (@total - 1) if finish_idx >= @total

      tot = finish_idx - start_idx + 1
      out = self.class.new
      out.tuple = Rubinius::Tuple.new(tot)
      out.total = tot
      out.tuple.copy_from(@tuple, @start + start_idx, tot, 0)
      return out
    end
  end

  alias_method :slice, :[]

  def set_index(index, ent, fin=Undefined)
    Ruby.primitive :array_aset

    ins_length = nil
    unless fin.equal? Undefined
      ins_length = Type.coerce_to ent, Fixnum, :to_int
      ent = fin             # 2nd arg (ins_length) is the optional one!
    end

    # Normalise Ranges
    if index.kind_of? Range
      if ins_length
        raise ArgumentError, "Second argument invalid with a range"
      end

      last = Type.coerce_to index.last, Fixnum, :to_int
      last += @total if last < 0
      last += 1 unless index.exclude_end?

      index = Type.coerce_to index.first, Fixnum, :to_int

      if index < 0
        index += @total
        raise RangeError, "Range begin #{index-@total} out of bounds" if index < 0
      end

      # m..n, m > n allowed
      last = index if index > last

      ins_length = last - index
    else
      index = Type.coerce_to index, Fixnum, :to_int

      if index < 0
        index += @total
        raise IndexError,"Index #{index-@total} out of bounds" if index < 0
      end
    end

    if ins_length
      # ins_length < 0 not allowed
      raise IndexError, "Negative length #{ins_length}" if ins_length < 0

      # MRI seems to be forgiving here!
      space = @total - index
      if ins_length > space
        ins_length = space > 0 ? space : 0
      end

      if ent.nil?
        # optimize for fast removal..
        reg_start = index + ins_length
        reg_length = @total - reg_start

        if reg_start <= @total
          @tuple.copy_from @tuple, reg_start, reg_length, index
          @total -= ins_length

          return ent
        end

        # This is actually an addition! silly, I know.
        replacement = []
      elsif ent.kind_of? Array
        replacement = ent
      elsif ent.respond_to? :to_ary
        replacement = ent.to_ary
      else
        replacement = [ent]
      end

      newtotal = (index > @total) ? index : @total
      if replacement.size > ins_length
        newtotal += replacement.size - ins_length
      elsif replacement.size < ins_length
        newtotal -= ins_length - replacement.size
      end

      nt = Rubinius::Tuple.new(newtotal)
      nt.copy_from(@tuple, @start, index < @total ? index : @total, 0)
      nt.copy_from(replacement.tuple, replacement.start, replacement.size, index)
      if index < @total
        nt.copy_from(@tuple, @start+index+ins_length, @total-index-ins_length,
                     index+replacement.size)
      end
      @start = 0
      @tuple = nt
      @total = newtotal

      return ent
    else
      nt = @start + index + 1
      reallocate(nt) if @tuple.size < nt

      @tuple.put @start + index, ent
      if index >= @total - 1
        @total = index + 1
      end
      return ent
    end
  end

  alias_method :[]=, :set_index

  private :set_index

  # Appends the object to the end of the Array.
  # Returns self so several appends can be chained.
  def <<(obj)
    set_index(@total, obj)
    self
  end

  # Creates a new Array containing only elements common to
  # both Arrays, without duplicates. Also known as a 'set
  # intersection'
  def &(other)
    other = Type.coerce_to other, Array, :to_ary

    array = []
    im = IdentityMap.new other

    i = to_iter
    while i.next
      item = i.item
      array << item if im.delete item
    end

    array
  end

  # Creates a new Array by combining the two Arrays' items,
  # without duplicates. Also known as a 'set union.'
  def |(other)
    other = Type.coerce_to other, Array, :to_ary

    im = IdentityMap.new self, other.size
    im.load other
    im.to_array
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

      sz = self.size()
      new_size = val * sz

      nt = Rubinius::Tuple.new(new_size)
      out = self.class.new()
      out.tuple = nt
      out.total = new_size
      out.taint if self.tainted? && val > 0

      i = 0
      while i < new_size
        nt.copy_from(@tuple,@start,@total, i)
        i += sz
      end
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

    array = []
    im = IdentityMap.new other

    i = to_iter
    while i.next
      item = i.item
      array << item unless im.include? item
    end

    array
  end

  # Compares the two Arrays and returns -1, 0 or 1 depending
  # on whether the first one is 'smaller', 'equal' or 'greater'
  # in relation to the second. Two Arrays are equal only if all
  # their elements are 0 using first_e <=> second_e and their
  # lengths are the same. The element comparison is the primary
  # and length is only checked if the former results in 0's.
  def <=>(other)
    other = Type.coerce_to other, Array, :to_ary
    return 0 if equal? other

    Thread.detect_recursion self, other do
      max = other.size < size ? other.size : size
      i = to_iter
      i.bounds 0, max
      while i.next
        diff = i.item <=> other.at(i.index)
        return diff if diff != 0
      end
    end
    # subtle: if we are recursing on that pair, then let's
    # no go any further down into that pair;
    # any difference will be found elsewhere if need be
    size <=> other.size
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
      i = to_iter
      while i.next
        return false unless i.item == other.at(i.index)
      end
    end

    true
  end

  # Assumes the Array contains other Arrays and searches through
  # it comparing the given object with the first element of each
  # contained Array using elem == obj. Returns the first contained
  # Array that matches (the first 'associated' Array) or nil.
  def assoc(obj)
    i = to_iter
    while i.next
      elem = i.item
      return elem if elem.kind_of? Array and elem.first == obj
    end

    nil
  end

  # Returns the element at the given index. If the
  # index is negative, counts from the end of the
  # Array. If the index is out of range, nil is
  # returned. Slightly faster than +Array#[]+
  def at(idx)
    Ruby.primitive :array_aref
    idx = Type.coerce_to idx, Fixnum, :to_int

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
    @tuple = Rubinius::Tuple.new(1)
    @total = 0
    @start = 0
    self
  end

  # Returns a copy of self with all nil elements removed
  def compact
    out = dup
    out.compact! || out
  end

  # Removes all nil elements from self, returns nil if no changes
  def compact!
    if (deleted = @tuple.delete(@start,@total,nil)) > 0
      @total -= deleted
      reallocate_shrink()
      return self
    else
      return nil
    end
  end

  # Appends the elements in the other Array to self
  def concat(other)
    ary = Type.coerce_to(other, Array, :to_ary)
    size = @total + ary.size
    tuple = Rubinius::Tuple.new size
    tuple.copy_from @tuple, @start, @total, 0 if @total > 0
    tuple.copy_from ary.tuple, ary.start, ary.size, @total
    @tuple = tuple
    @start = 0
    @total = size
    self
  end

  # Removes all elements from self that are #== to the given object.
  # If the object does not appear at all, nil is returned unless a
  # block is provided in which case the value of running it is
  # returned instead.
  def delete(obj)
    key = Undefined
    i = to_iter
    while i.next
      self[i.index] = key if i.item == obj
    end

    deleted = @tuple.delete @start, @total, key
    if deleted > 0
      @total -= deleted
      reallocate_shrink()
      return obj
    else
      if block_given?
        yield
      else
        nil
      end
    end
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

    @tuple.copy_from(@tuple, @start+idx+1, @total-idx-1, @start+idx)
    @tuple.put(@start + @total - 1, nil)

    @total -= 1
    obj
  end

  # Deletes every element from self for which block evaluates to true
  #
  # NOTE: This method is overridden by lib/1.8.7 or lib/1.9.
  def delete_if(&block)
    key = Undefined
    i = to_iter
    while i.next
      self[i.index] = key if yield i.item
    end

    deleted = @tuple.delete @start, @total, key
    if deleted > 0
      @total -= deleted
      reallocate_shrink()
    end
    return self
  end

  # Passes each index of the Array to the given block
  # and returns self.  We re-evaluate @total each time
  # through the loop in case the array has changed.
  #
  # NOTE: This method is overridden by lib/1.8.7 or lib/1.9.
  def each_index
    i = to_iter
    while i.next
      yield i.index
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
      i = to_iter
      while i.next
        return false unless i.item.eql? other.at(i.index)
      end
    end

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
    raise ArgumentError, "Wrong number of arguments" if !block_given? and args.size == 0
    raise ArgumentError, "Wrong number of arguments" if args.size > 3

    # Normalise arguments
    start, finish, obj = 0, (@total - 1), nil

    obj = args.shift unless block_given?
    one, two = args.at(0), args.at(1)

    if one.kind_of? Range
      raise TypeError, "Length invalid with range" if args.size > 1   # WTF, MRI, TypeError?

      start  = Type.coerce_to one.begin, Fixnum, :to_int
      finish = Type.coerce_to one.end, Fixnum, :to_int

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
  def first(n = Undefined)
    return at(0) if n.equal? Undefined

    n = Type.coerce_to n, Fixnum, :to_int
    raise ArgumentError, "Size must be positive" if n < 0

    self[0...n].to_a
  end

  # Recursively flatten any contained Arrays into an one-dimensional result.
  def flatten
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
  def hash
    # IMPROVE: This is a really really poor implementation of hash for an array, but
    # it does work. It should be replaced with something much better, but I'm not sure
    # what level it belongs at.
    str = ""
    return @total if Thread.detect_recursion self do
      i = 0
      while(i < @total)
        curr = at(i)
        str.append curr.hash.to_s
        i+=1
      end
    end
    str.hash
  end

  # Returns true if the given obj is present in the Array.
  # Presence is determined by calling elem == obj until found.
  def include?(obj)
    i = to_iter
    while i.next
      return true if i.item == obj
    end
    false
  end

  # Returns the index of the first element in the Array
  # for which elem == obj is true or nil.
  def index(obj)
    i = to_iter
    while i.next
      return i.index if i.item == obj
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
    out = []
    return "[...]" if Thread.detect_recursion self do
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
  def join(sep=nil, method=:to_s)
    return "" if @total == 0
    out = ""
    return "[...]" if Thread.detect_recursion self do
      sep = sep ? StringValue(sep) : $,
      out.taint if sep.tainted? or self.tainted?
      i = to_iter
      while i.next
        elem = i.item

        out.append sep unless i.index == 0

        if elem.kind_of?(Array)
          out.append elem.join(sep, method)
        else
          out.append elem.to_s
        end

        out.taint if elem.tainted?
      end
    end

    out
  end

  # Returns the last element or n elements of self. If
  # the Array is empty, without a count nil is returned,
  # otherwise an empty Array. Always returns an Array.
  def last(n=Undefined)
    if @total < 1
      return if n.equal? Undefined
      return []
    end
    return at(-1) if n.equal? Undefined

    unless n.respond_to?(:to_int)
      raise TypeError, "Can't convert #{n.class} into Integer"
    end

    n = Type.coerce_to n, Fixnum, :to_int
    return [] if n.zero?
    raise ArgumentError, "Number must be positive" if n < 0

    n = size if n > size
    self[-n..-1].to_a
  end

  alias_method :collect, :map

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
    # see pack.rb for Array::Packer
    Packer.new(self,schema).dispatch
  end

  # Removes and returns the last element from the Array.
  def pop
    return nil if empty?

    @total -= 1
    index = @start + @total

    elem = @tuple.at(index)
    @tuple.put(index,nil)

    reallocate_shrink()

    elem
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
    each do |elem|
      if elem.kind_of? Array and elem.at(1) == obj
        return elem
      end
    end

    nil
  end

  # Returns a new Array by removing items from self for
  # which block is true. An Array is also returned when
  # invoked on subclasses. See #reject!
  #
  # NOTE: This method is overridden by lib/1.8.7 or lib/1.9.
  def reject(&block)
    Array.new(self).reject!(&block) || self
  end

  # Equivalent to #delete_if except that returns nil if
  # no changes were made.
  #
  # NOTE: This method is overridden by lib/1.8.7 or lib/1.9.
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
  def reverse
    dup.reverse!
  end

  # Reverses the order of elements in self. Returns self
  # even if no changes are made
  def reverse!
    return self unless @total > 1

    i = @start
    j = @start + @total - 1
    while i < (@start+@total/2)
      @tuple.swap(i,j)
      i += 1
      j -= 1
    end
    self
  end

  # Goes through the Array back to front and yields
  # each element to the supplied block. Returns self.
  #
  # NOTE: This method is overridden by lib/1.8.7 or lib/1.9.
  def reverse_each
    i = to_reverse_iter
    while i.rnext
      yield i.item
    end
    self
  end

  # Returns the index of the last element in the Array
  # for which elem == obj is true.
  def rindex(obj)
    i = to_reverse_iter
    while i.rnext
      return i.index if i.item == obj
    end
    nil
  end

  # Removes and returns the first element in the
  # Array or nil if empty. All other elements are
  # moved down one index.
  def shift
    return nil if @total == 0

    obj = @tuple.at @start
    @tuple.put @start, nil
    @start += 1
    @total -= 1

    reallocate_shrink()

    obj
  end

  # Deletes the element(s) given by an index (optionally with a length)
  # or by a range. Returns the deleted object, subarray, or nil if the
  # index is out of range. Equivalent to:
  def slice!(start, length=Undefined)
    if length.equal? Undefined
      out = self[start]

      if start.kind_of? Range
        self[start] = nil
      else
        # make sure that negative values are not passed through to the
        # []= assignment
        start = Type.coerce_to start, Integer, :to_int
        start = start + self.length if start < 0

        # This is to match the MRI behaviour of not extending the array
        # with nil when specifying an index greater than the length
        # of the array.
        return out unless start >= 0 and start < self.length
        self[start, 1] = nil
      end
    else
      out = self[start,length]
      self[start,length] = nil
    end

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
  def to_a
    if self.class == Array
      self
    else
      Array.new(self)
    end
  end

  # Returns self
  def to_ary
    self
  end

  # Produces a string by joining all elements without a
  # separator. See #join
  def to_s
    self.join
  end

  # Treats all elements as being Arrays of equal lengths and
  # transposes their rows and columns so that the first contained
  # Array in the result includes the first elements of all the
  # Arrays and so on.
  def transpose
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
  def uniq
    dup.uniq! or dup
  end

  # Removes duplicates from the Array in place as #uniq
  def uniq!
    im = IdentityMap.new self
    return if im.size == size
    im.to_array self
  end

  # Returns a new Array populated from the elements in
  # self corresponding to the given selector(s.) The
  # arguments may be one or more integer indices or
  # Ranges.
  def values_at(*args)
    out = []

    args.each { |elem|
      # Cannot use #[] because of subtly different errors
      if elem.kind_of? Range
        finish = Type.coerce_to elem.last, Fixnum, :to_int
        start = Type.coerce_to elem.first, Fixnum, :to_int

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

  #  call-seq:
  #     array.zip(arg, ...)                   -> an_array
  #     array.zip(arg, ...) {| arr | block }  -> nil
  #
  #  Converts any arguments to arrays, then merges elements of
  #  <i>self</i> with corresponding elements from each argument. This
  #  generates a sequence of <code>self.size</code> <em>n</em>-element
  #  arrays, where <em>n</em> is one more that the count of arguments. If
  #  the size of any argument is less than <code>enumObj.size</code>,
  #  <code>nil</code> values are supplied. If a block given, it is
  #  invoked for each output array, otherwise an array of arrays is
  #  returned.
  #
  #     a = [ 4, 5, 6 ]
  #     b = [ 7, 8, 9 ]
  #     [1,2,3].zip(a, b)      #=> [[1, 4, 7], [2, 5, 8], [3, 6, 9]]
  #     [1,2].zip(a,b)         #=> [[1, 4, 7], [2, 5, 8]]
  #     a.zip([1,2],[8])       #=> [[4,1,8], [5,2,nil], [6,nil,nil]]
  #
  def zip(*others)
    out = Array.new(size) { [] }
    others = others.map { |ary| ary.to_ary }

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

  def unshift(*values)
    if @start > values.size
      # fit the new values in between 0 and @start if possible
      @start -= values.size
      @tuple.copy_from(values.tuple,0,values.size,@start)
    else
      # FIXME: provision for more unshift prepends?
      tuple = Rubinius::Tuple.new(@total+values.size)
      tuple.copy_from(values.tuple,0,values.size,0)
      tuple.copy_from(@tuple,@start,@total,values.size)
      @start = 0
      @tuple = tuple
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

    tuple = Rubinius::Tuple.new(new_size)
    tuple.copy_from @tuple, @start, @total, 0

    @start = 0
    @tuple = tuple
  end

  private :reallocate

  def reallocate_shrink
    new_size = @tuple.size
    return if @total > (new_size / 3)

    # halve the tuple size until the total > 1/3 the size of the total
    begin
      new_size /= 2
    end while @total < (new_size / 6)

    tuple = Rubinius::Tuple.new(new_size)
    # position values in the middle somewhere
    new_start = (new_size-@total)/2
    tuple.copy_from(@tuple, @start, @total, new_start)

    @start = new_start
    @tuple = tuple
  end

  private :reallocate_shrink

  # Helper to recurse through flattening since the method
  # is not allowed to recurse itself. Detects recursive structures.
  def recursively_flatten(array, out, max_levels = -1)
    ret = nil
    if max_levels == 0  # Strict equality since < 0 means 'infinite'
      out.concat(array)
    else
      max_levels -= 1
      recursion = Thread.detect_recursion(array) do
        array.each { |o|
          if o.respond_to? :to_ary
            ary = Type.coerce_to o, Array, :to_ary
            recursively_flatten(ary, out, max_levels)
            ret = self
          else
            out << o
          end
        }
      end
      raise ArgumentError, "tried to flatten recursive array" if recursion
    end
    ret
  end

  private :recursively_flatten

  ISORT_THRESHOLD   = 7
  MEDIAN_THRESHOLD  = 11

  # In-place non-recursive sort between the given indexes.
  def qsort!
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
      if segment_size > 1000 and (low <=> mid) > 0 and (mid <=> hi) > 0
        semi_left = @tuple.at(left_end + ((middle - left_end) / 2))
        semi_right = @tuple.at(middle + ((right_end - middle) / 2))

        if (low <=> semi_left) > 0 and (semi_left <=> mid) > 0 and
           (mid <=> semi_right) > 0 and (semi_right <=> hi) > 0

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
          cmp = @tuple.at(left) <=> pivot
          break if cmp > 0
          @tuple.swap(left, (eqls_left += 1)) if cmp == 0
          left += 1
        end

        while right > left_end
          cmp = @tuple.at(right) <=> pivot
          break if cmp < 0
          @tuple.swap(right, (eqls_right -= 1)) if cmp == 0
          right -= 1
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
  end # qsort!
  private :qsort!

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
      if segment_size > 1000 and block.call(low, mid) > 0 and block.call(mid, hi) > 0
        semi_left = @tuple.at(left_end + ((middle - left_end) / 2))
        semi_right = @tuple.at(middle + ((right_end - middle) / 2))

        if block.call(low, semi_left) > 0 and block.call(semi_left, mid) > 0 and
           block.call(mid, semi_right) > 0 and block.call(semi_right, hi) > 0

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
          cmp = block.call(@tuple.at(left), pivot)
          break if cmp > 0
          @tuple.swap(left, (eqls_left += 1)) if cmp == 0
          left += 1
        end

        while right > left_end
          cmp = block.call(@tuple.at(right), pivot)
          break if cmp < 0
          @tuple.swap(right, (eqls_right -= 1)) if cmp == 0
          right -= 1
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
  end #qsort_block!
  private :qsort_block!

  # Insertion sort in-place between the given indexes.
  def isort!(left, right)
    i = left + 1

    while i <= right
      j = i

      while j > @start and (@tuple.at(j - 1) <=> @tuple.at(j)) > 0
        @tuple.swap(j, (j - 1))
        j -= 1
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

      while j > @start and block.call(@tuple.at(j - 1), @tuple.at(j)) > 0
        @tuple.swap(j, (j - 1))
        j -= 1
      end

      i += 1
    end
  end
  private :isort_block!

  def __rescue_match__(exception)
    i = to_iter
    while i.next
      return true if i.item === exception
    end
    false
  end
end
