##
# A Range represents an interval, a set of values with a start and an end.
#
# Ranges may be constructed using the <tt>s..e</tt> and <tt>s...e</tt>
# literals, or with Range::new.
#
# Ranges constructed using <tt>..</tt> run from the start to the end
# inclusively. Those created using <tt>...</tt> exclude the end value. When
# used as an iterator, ranges return each value in the sequence.
#
#   (-1..-5).to_a      #=> []
#   (-5..-1).to_a      #=> [-5, -4, -3, -2, -1]
#   ('a'..'e').to_a    #=> ["a", "b", "c", "d", "e"]
#   ('a'...'e').to_a   #=> ["a", "b", "c", "d"]
#
# Ranges can be constructed using objects of any type, as long as the objects
# can be compared using their <tt><=></tt> operator and they support the
# <tt>succ</tt> method to return the next object in sequence.
#
#   class Xs # represent a string of 'x's
#     include Comparable
#     attr :length
#     def initialize(n)
#       @length = n
#     end
#     def succ
#       Xs.new(@length + 1)
#     end
#     def <=>(other)
#       @length <=> other.length
#     end
#     def to_s
#       sprintf "%2d #{inspect}", @length
#     end
#     def inspect
#       'x'# @length
#     end
#   end
#   
#   r = Xs.new(3)..Xs.new(6)   #=> xxx..xxxxxx
#   r.to_a                     #=> [xxx, xxxx, xxxxx, xxxxxx]
#   r.member?(Xs.new(5))       #=> true
#
# In the previous code example, class Xs includes the Comparable module. This
# is because Enumerable#member? checks for equality using ==. Including
# Comparable ensures that the == method is defined in terms of the <=> method
# implemented in Xs.

class Range
  include Enumerable

  ##
  # Constructs a range using the given +start+ and +end+.
  #
  # If the third parameter is omitted or is false, the range will include the
  # end object; otherwise, it will be excluded.

  def initialize(first, last, exclude_end = false)
    raise NameError, "`initialize' called twice" if @begin
    
    unless first.kind_of?(Fixnum) && last.kind_of?(Fixnum)
      begin
        raise ArgumentError, "bad value for range" unless first <=> last
      rescue
        raise ArgumentError, "bad value for range"
      end
    end
    
    @begin = first
    @end = last
    @excl = exclude_end
  end

  private :initialize

  # Returns <tt>true</tt> only if <em>obj</em> is a Range, has
  # equivalent beginning and end items (by comparing them with
  # <tt>==</tt>), and has the same #exclude_end? setting as <i>rng</t>.
  #
  #   (0..2) == (0..2)            #=> true
  #   (0..2) == Range.new(0,2)    #=> true
  #   (0..2) == (0...2)           #=> false
  def ==(other)
    return true if equal? other

    other.kind_of?(Range) and
      self.first == other.first and
      self.last == other.last and
      self.exclude_end? == other.exclude_end?
   
  end
  alias_method :eql?, :==

  # Returns <tt>true</tt> if <em>obj</em> is an element of <em>rng</em>,
  # <tt>false</tt> otherwise. Conveniently, <tt>===</tt> is the
  # comparison operator used by <tt>case</tt> statements.
  #
  #   case 79
  #     when 1..50   then   print "low\n"
  #     when 51..75  then   print "medium\n"
  #     when 76..100 then   print "high\n"
  #   end
  #
  # <em>produces:</em>
  #
  #   high
  def ===(value)
    # MRI uses <=> to compare, so must we.

    beg_compare = (@begin <=> value)
    return false unless beg_compare

    if Comparable.compare_int(beg_compare) <= 0
      end_compare = (value <=> @end)
      if @excl
        return true if Comparable.compare_int(end_compare) < 0
      else
        return true if Comparable.compare_int(end_compare) <= 0
      end
    end

    return false
  end
  alias_method :member?, :===
  alias_method :include?, :===

  ##
  # :call-seq:
  #   rng.exclude_end?  => true or false
  #
  # Returns true if +rng+ excludes its end value.

  attr_reader_specific :excl, :exclude_end?

  ##
  # :call-seq:
  #   rng.each { |i| block }  => rng
  #
  # Iterates over the elements +rng+, passing each in turn to the block. You
  # can only iterate if the start object of the range supports the
  # succ method (which means that you can't iterate over ranges of
  # Float objects).
  #
  #   (10..15).each do |n|
  #      print n, ' '
  #   end
  #
  # produces:
  #
  #   10 11 12 13 14 15

  def each(&block)
    return to_enum unless block_given?
    first, last = @begin, @end

    raise TypeError, "can't iterate from #{first.class}" unless first.respond_to? :succ

    case first
    when Fixnum
      last -= 1 if @excl

      i = first
      while i <= last
        yield i
        i += 1
      end

    when String
      first.upto(last) do |s|
        yield s unless @excl && s == last
      end
    else
      current = first
      if @excl
        while (current <=> last) < 0
          yield current
          current = current.succ
        end
      else
        while (c = current <=> last) && c <= 0
          yield current
          break if c == 0
          current = current.succ
        end
      end
    end
    return self
  end

  ##
  # :call-seq:
  #   rng.first  => obj
  #   rng.begin  => obj
  #
  # Returns the first object in +rng+.

  attr_reader :begin
  alias_method :first, :begin

  # Generate a hash value such that two ranges with the same start and
  # end points, and the same value for the "exclude end" flag, generate
  # the same hash value.
  def hash
    excl = @excl ? 1 : 0
    hash = excl
    hash ^= @begin.hash << 1
    hash ^= @end.hash << 9
    hash ^= excl << 24;
    return hash
  end

  # Convert this range object to a printable form (using
  # <tt>inspect</tt> to convert the start and end objects).
  def inspect
    "#{@begin.inspect}#{@excl ? "..." : ".."}#{@end.inspect}"
  end

  # Returns the object that defines the end of <em>rng</em>.
  #
  #    (1..10).end    #=> 10
  #    (1...10).end   #=> 10
  attr_reader :end
  alias_method :last, :end

  ##
  # :call-seq:
  #   rng.step(n = 1) { |obj| block }  => rng
  #
  # Iterates over +rng+, passing each +n+th element to the block. If the range
  # contains numbers or strings, natural ordering is used. Otherwise
  # +step+ invokes +succ+ to iterate through range elements. The following
  # code uses class Xs, which is defined in the class-level documentation.
  #
  #   range = Xs.new(1)..Xs.new(10)
  #   range.step(2) { |x| puts x }
  #   range.step(3) { |x| puts x }
  #
  # produces:
  #
  #    1 x
  #    3 xxx
  #    5 xxxxx
  #    7 xxxxxxx
  #    9 xxxxxxxxx
  #    1 x
  #    4 xxxx
  #    7 xxxxxxx
  #   10 xxxxxxxxxx

  def step(step_size=1) # :yields: object
    return to_enum(:step, step_size) unless block_given?
    first = @begin
    last = @end

    if first.kind_of? Float
      step_size = Float(step_size)
    else
      step_size = Integer(step_size)
    end

    if step_size <= 0
      raise ArgumentError, "step can't be negative" if step_size < 0
      raise ArgumentError, "step can't be 0"
    end

    if first.kind_of?(Numeric)
      last -= 1 if @excl

      while first <= last
        yield first
        first += step_size
      end
    else
      counter = 0
      each do |o|
        yield o if counter % step_size == 0
        counter += 1
      end
    end

    return self
  end

  ##
  # Convert this range object to a printable form.

  def to_s
    "#{@begin}#{@excl ? "..." : ".."}#{@end}"
  end

  def to_a
    if @begin.kind_of? Fixnum and @end.kind_of? Fixnum
      fin = @end
      fin += 1 unless @excl

      size = fin - @begin

      ary = Array.new(size)
      i = 0
      while i < size
        ary[i] = @begin + i
        i += 1
      end

      return ary
    end

    super
  end

end

