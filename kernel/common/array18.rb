class Array
  # Choose a random element from the array.
  def choice
    at Kernel.rand(size)
  end

  # Recursively flatten any contained Arrays into an one-dimensional result.
  # The optional level argument determines the level of recursion to flatten
  def flatten(level=-1)
    level = Rubinius::Type.coerce_to(level, Integer, :to_int)
    return self if level == 0

    out = new_reserved size
    recursively_flatten(self, out, level)
    out
  end

  # Flattens self in place as #flatten. If no changes are
  # made, returns nil, otherwise self.
  # The optional level argument determines the level of recursion to flatten
  def flatten!(level=-1)
    level = Rubinius::Type.coerce_to(level, Integer, :to_int)
    return nil if level == 0

    out = new_reserved size
    if recursively_flatten(self, out, level)
      replace(out)
      return self
    end

    nil
  end

  # For a positive index, inserts the given values before
  # the element at the given index. Negative indices count
  # backwards from the end and the values are inserted
  # after them.
  def insert(idx, *items)
    return self if items.length == 0

    Rubinius.check_frozen

    # Adjust the index for correct insertion
    idx = Rubinius::Type.coerce_to idx, Fixnum, :to_int
    idx += (@total + 1) if idx < 0    # Negatives add AFTER the element
    raise IndexError, "#{idx} out of bounds" if idx < 0

    self[idx, 0] = items   # Cheat
    self
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

  def pack(directives)
    Rubinius.primitive :array_pack18

    unless directives.kind_of? String
      return pack(StringValue(directives))
    end

    raise ArgumentError, "invalid directives string: #{directives}"
  end

  # Appends the given object(s) to the Array and returns
  # the modified self.
  def push(*args)
    return self if args.empty?

    concat args
  end

  # Produces a string by joining all elements without a
  # separator. See #join
  def to_s
    join
  end

  # Returns a new Array by removing duplicate entries
  # from self. Equality is determined by using a Hash
  def uniq
    dup.uniq! or dup
  end

  # Removes duplicates from the Array in place as #uniq
  def uniq!
    im = Rubinius::IdentityMap.from self
    return if im.size == size

    Rubinius.check_frozen

    array = im.to_array
    @tuple = array.tuple
    @start = array.start
    @total = array.total

    self
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
    return self if values.empty?

    Rubinius.check_frozen

    if @start > values.size
      # fit the new values in between 0 and @start if possible
      @start -= values.size
      @tuple.copy_from(values.tuple,0,values.size,@start)
    else
      new_tuple = Rubinius::Tuple.new @total + values.size
      new_tuple.copy_from values.tuple, 0, values.size, 0
      new_tuple.copy_from @tuple, @start, @total, values.size
      @start = 0
      @tuple = new_tuple
    end

    @total += values.size
    self
  end

  # Returns a copy of self with all nil elements removed
  def compact
    out = dup
    out.compact! || out
  end
end
