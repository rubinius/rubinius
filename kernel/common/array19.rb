class Array
  # Try to convert obj into an array, using to_ary method.
  # Returns converted array or nil if obj cannot be converted
  # for any reason. This method is to check if an argument is an array.
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Array, :to_ary
  end

  # Repetition operator when supplied a #to_int argument:
  # returns a new Array as a concatenation of the given number
  # of the original Arrays. With an argument that responds to
  # #to_str, functions exactly like #join instead.
  def *(multiplier)
    if multiplier.respond_to? :to_str
      return join(multiplier)

    else
      # Aaargh stupid MRI's stupid specific stupid error stupid types stupid
      multiplier = Rubinius::Type.coerce_to multiplier, Fixnum, :to_int

      raise ArgumentError, "Count cannot be negative" if multiplier < 0

      case @total
      when 0
        # Edge case
        out = self.class.allocate
        out.taint if tainted?
        out.untrust if untrusted?
        return out
      when 1
        # Easy case
        tuple = Rubinius::Tuple.pattern multiplier, at(0)
        out = self.class.allocate
        out.tuple = tuple
        out.total = multiplier
        out.taint if tainted?
        out.untrust if untrusted?
        return out
      end

      new_total = multiplier * @total
      new_tuple = Rubinius::Tuple.new(new_total)

      out = self.class.allocate
      out.tuple = new_tuple
      out.total = new_total
      out.taint if tainted?
      out.untrust if untrusted?

      offset = 0
      while offset < new_total
        new_tuple.copy_from @tuple, @start, @total, offset
        offset += @total
      end

      out
    end
  end


  # Appends the elements in the other Array to self
  def concat(other)
    Rubinius.primitive :array_concat

    other = Rubinius::Type.coerce_to(other, Array, :to_ary)
    Rubinius.check_frozen

    return self if other.empty?

    concat other
  end

  def flatten(level=-1)
    level = Rubinius::Type.coerce_to(level, Integer, :to_int)
    return self.dup if level == 0

    out = new_reserved size
    recursively_flatten(self, out, level)
    out
  end

  # Flattens self in place as #flatten. If no changes are
  # made, returns nil, otherwise self.
  # The optional level argument determines the level of recursion to flatten
  def flatten!(level=-1)
    Rubinius.check_frozen

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
    Rubinius.check_frozen

    return self if items.length == 0

    # Adjust the index for correct insertion
    idx = Rubinius::Type.coerce_to idx, Fixnum, :to_int
    idx += (@total + 1) if idx < 0    # Negatives add AFTER the element
    raise IndexError, "#{idx} out of bounds" if idx < 0

    self[idx, 0] = items   # Cheat
    self
  end

  # Produces a printable string of the Array. The string
  # is constructed by calling #inspect on all elements.
  # Descends through contained Arrays, recursive ones
  # are indicated as [...].
  def inspect
    return "[]" if @total == 0

    comma = ", "
    result = "["

    return "[...]" if Thread.detect_recursion self do
      each { |o| result << o.inspect << comma }
    end

    result.taint if tainted?
    result.untrust if untrusted?
    result.shorten!(2)
    result << "]"
  end

  def keep_if(&block)
    Rubinius.check_frozen

    return to_enum :keep_if unless block_given?

    replace select(&block)
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
    Rubinius.primitive :array_pack19

    unless directives.kind_of? String
      return pack(StringValue(directives))
    end

    raise ArgumentError, "invalid directives string: #{directives}"
  end

  # Returns an array of all combinations of elements from all arrays.  The
  # length of the returned array is the product of the length of ary and the
  # argument arrays
  #
  # --
  # Implementation notes: We build a block that will generate all the
  # combinations by building it up successively using "inject" and starting
  # with one responsible to append the values.
  # ++
  def product(*args, &block)
    args.map! { |x| Rubinius::Type.coerce_to(x, Array, :to_ary) }

    # Check the result size will fit in an Array.
    sum = args.inject(size) { |n, x| n * x.size }

    if sum > Fixnum::MAX
      raise RangeError, "product result is too large"
    end

    # TODO rewrite this to not use a tree of Proc objects.

    # to get the results in the same order as in MRI, vary the last argument first
    args.reverse!

    result = []
    args.push self

    outer_lambda = args.inject(result.method(:push)) do |trigger, values|
      lambda do |partial|
        values.each do |val|
          trigger.call(partial.dup << val)
        end
      end
    end

    outer_lambda.call([])

    if block_given?
      block_result = self
      result.each {|v| block_result << yield(v) }
      block_result
    else
      result
    end
  end

  # Appends the given object(s) to the Array and returns
  # the modified self.
  def push(*args)
    Rubinius.check_frozen

    return self if args.empty?

    concat args
  end

  # Replaces contents of self with contents of other,
  # adjusting size as needed.
  def replace(other)
    Rubinius.check_frozen

    other = Rubinius::Type.coerce_to other, Array, :to_ary

    @tuple = other.tuple.dup
    @total = other.total
    @start = other.start

    untrust if other.untrusted?

    self
  end

  alias_method :initialize_copy, :replace
  private :initialize_copy

  def rotate(n=1)
    return self.dup if length == 1
    return []       if empty?

    ary = self.dup
    idx = n % ary.size

    ary[idx..-1].concat ary[0...idx]
  end

  def rotate!(cnt=1)
    Rubinius.check_frozen

    return self if length == 0 || length == 1

    ary = rotate(cnt)
    replace ary
  end

  # Choose a random element, or the random n elements, from the array.
  # If the array is empty, the first form returns nil, and the second
  # form returns an empty array.
  def sample(n=undefined)
    return at(Kernel.rand(size)) if n.equal? undefined

    n = Rubinius::Type.coerce_to(n, Fixnum, :to_int)
    raise ArgumentError, "negative array size" if n < 0

    n = size if n > size
    result = Array.new(self)

    n.times do |i|
      r = i + Kernel.rand(size - i)
      result.tuple.swap(i,r)
    end

    result[n..size] = []
    result
  end

  def select!(&block)
    Rubinius.check_frozen

    return to_enum :select! unless block_given?

    ary = select(&block)
    replace ary unless size == ary.size
  end

  def sort_by!(&block)
    Rubinius.check_frozen

    return to_enum :sort_by! unless block_given?

    replace sort_by(&block)
  end

  alias_method :to_s, :inspect

  def uniq(&block)
    dup.uniq!(&block) or dup
  end

  def uniq!(&block)
    Rubinius.check_frozen

    if block_given?
      im = Rubinius::IdentityMap.from(self, &block)
    else
      im = Rubinius::IdentityMap.from(self)
    end
    return if im.size == size

    array = im.to_array
    @tuple = array.tuple
    @start = array.start
    @total = array.total

    self
  end

  #  call-seq:
  #     ary.zip(arg, ...)                   -> new_ary
  #     ary.zip(arg, ...) {| arr | block }  -> nil
  #
  #  Converts any arguments to arrays, then merges elements of
  #  +self+ with corresponding elements from each argument. This
  #  generates a sequence of <code>self.size</code> <em>n</em>-element
  #  arrays, where <em>n</em> is one more that the count of arguments. If
  #  the size of any argument is less than <code>enumObj.size</code>,
  #  <code>nil</code> values are supplied. If a block is given, it is
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
    others = others.map do |ary|
      if ary.respond_to?(:to_ary)
        ary.to_ary
      else
        elements = []
        ary.each { |e| elements << e }
        elements
      end
    end

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
    Rubinius.check_frozen

    return self if values.empty?

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
end
