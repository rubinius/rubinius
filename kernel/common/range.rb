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
    
    unless first.is_a?(Fixnum) && last.is_a?(Fixnum)
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

  # Returns <tt>true</tt> only if <em>obj</em> is a Range, has
  # equivalent beginning and end items (by comparing them with
  # <tt>==</tt>), and has the same #exclude_end? setting as <i>rng</t>.
  #
  #   (0..2) == (0..2)            #=> true
  #   (0..2) == Range.new(0,2)    #=> true
  #   (0..2) == (0...2)           #=> false
  def ==(other)
    self.equal?(other) ||
      (other.is_a?(Range) && self.first == other.first &&
       self.last == other.last && self.exclude_end? == other.exclude_end?)
   
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
    if @begin <= value
      if self.exclude_end?
        return true if value < @end
      else
        return true if value <= @end
      end
    end
    return false
  rescue
    return false
  end
  alias_method :member?, :===
  alias_method :include?, :===

  ##
  # :call-seq:
  #   rng.exclude_end?  => true or false
  #
  # Returns true if +rng+ excludes its end value.

  def exclude_end?
    @excl
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
  # Convert this range object to a printable form.

  def to_s
    "#{@begin}#{@excl ? "..." : ".."}#{@end}"
  end

end

