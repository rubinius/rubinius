# depends on: enumerable.rb

##
# Comments borrowed from MRI Implementation
#
# A <code>Range</code> represents an interval---a set of values with a
# start and an end. Ranges may be constructed using the
#  <em>s</em><code>..</code><em>e</em> and
#  <em>s</em><code>...</code><em>e</em> literals, or with
#  <code>Range::new</code>. Ranges constructed using <code>..</code>
#  run from the start to the end inclusively. Those created using
#  <code>...</code> exclude the end value. When used as an iterator,
#  ranges return each value in the sequence.
#     
#     (-1..-5).to_a      #=> []
#     (-5..-1).to_a      #=> [-5, -4, -3, -2, -1]
#     ('a'..'e').to_a    #=> ["a", "b", "c", "d", "e"]
#     ('a'...'e').to_a   #=> ["a", "b", "c", "d"]
#     
#  Ranges can be constructed using objects of any type, as long as the
#  objects can be compared using their <code><=></code> operator and
#  they support the <code>succ</code> method to return the next object
#  in sequence.
#     
#     class Xs                # represent a string of 'x's
#       include Comparable
#       attr :length
#       def initialize(n)
#         @length = n
#       end
#       def succ
#         Xs.new(@length + 1)
#       end
#       def <=>(other)
#         @length <=> other.length
#       end
#       def to_s
#         sprintf "%2d #{inspect}", @length
#       end
#       def inspect
#         'x'# @length
#       end
#     end
#     
#     r = Xs.new(3)..Xs.new(6)   #=> xxx..xxxxxx
#     r.to_a                     #=> [xxx, xxxx, xxxxx, xxxxxx]
#     r.member?(Xs.new(5))       #=> true
#     
#  In the previous code example, class <code>Xs</code> includes the
#  <code>Comparable</code> module. This is because
#  <code>Enumerable#member?</code> checks for equality using
#  <code>==</code>. Including <code>Comparable</code> ensures that the
#  <code>==</code> method is defined in terms of the <code><=></code>
#  method implemented in <code>Xs</code>.
class Range
  include Enumerable

  # Constructs a range using the given <em>start</em> and <em>end</em>.
  # If the third parameter is omitted or is <tt>false</tt>, the
  # <em>range</em> will include the end object; otherwise, it will be
  # excluded.
  def initialize(first, last, exclude_end = false)
    raise NameError, "`initialize' called twice" if @first
    
    unless first.is_a?(Fixnum) && last.is_a?(Fixnum)
      begin
        raise ArgumentError, "bad value for range" unless first <=> last
      rescue
        raise ArgumentError, "bad value for range"
      end
    end
    
    @first, @last = first, last
    @exclude_end  = exclude_end
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
    if @first <= value
      if self.exclude_end?
        return true if value < @last
      else
        return true if value <= @last
      end
    end
    return false
  rescue
    return false
  end
  alias_method :member?, :===
  alias_method :include?, :===

  # Iterates over the elements <em>rng</em>, passing each in turn to the
  # block. You can only iterate if the start object of the range
  # supports the <tt>succ</tt> method (which means that you can't
  # iterate over ranges of <tt>Float</tt> objects).
  #
  #    (10..15).each do |n|
  #       print n, ' '
  #    end
  #
  # <em>produces:</em>
  #
  #    10 11 12 13 14 15
  def each(&block)
    first, last = @first, @last # dup?
    
    raise TypeError, "can't iterate from #{first.class}" unless first.respond_to? :succ

    if first.is_a?(Fixnum) && last.is_a?(Fixnum)
      last -= 1 if self.exclude_end?
      first.upto(last, &block)
    elsif first.is_a?(String)
      first.upto(last) do |s|
        block.call(s) unless @exclude_end && s == last
      end
    else
      current = first
      if @exclude_end then
        while (current <=> last) < 0
          block.call(current)
          current = current.succ
        end
      else
        while (c = current <=> last) && c <= 0
          block.call(current)
          break if c == 0
          current = current.succ
        end
      end
    end
    return self
  end

  # Returns <tt>true</tt> if <em>rng</em> excludes its end value.
  def exclude_end?
    @exclude_end
  end

  # Returns the first object in <em>rng</em>.
  def first
    @first
  end
  alias_method :begin, :first

  # Generate a hash value such that two ranges with the same start and
  # end points, and the same value for the "exclude end" flag, generate
  # the same hash value.
  def hash
    excl = @exclude_end ? 1 : 0
    hash = excl
    hash ^= @first.hash << 1
    hash ^= @last.hash << 9
    hash ^= excl << 24;
    return hash
  end

  # Convert this range object to a printable form (using
  # <tt>inspect</tt> to convert the start and end objects).
  def inspect
    "#{@first.inspect}#{@exclude_end ? "..." : ".."}#{@last.inspect}"
  end

  # Returns the object that defines the end of <em>rng</em>.
  #
  #    (1..10).end    #=> 10
  #    (1...10).end   #=> 10
  def last
    return @last
  end
  alias_method :end, :last

  # Iterates over <em>rng</em>, passing each <em>n</em>th element to the
  # block. If the range contains numbers or strings, natural ordering is
  # used. Otherwise <tt>step</tt> invokes <tt>succ</tt> to iterate
  # through range elements. The following code uses class <tt>Xs</tt>,
  # which is defined in the class-level documentation.
  #
  #    range = Xs.new(1)..Xs.new(10)
  #    range.step(2) {|x| puts x}
  #    range.step(3) {|x| puts x}
  #
  # <em>produces:</em>
  #
  #     1 x
  #     3 xxx
  #     5 xxxxx
  #     7 xxxxxxx
  #     9 xxxxxxxxx
  #     1 x
  #     4 xxxx
  #     7 xxxxxxx
  #    10 xxxxxxxxxx
  def step(step_size = 1, &block) # :yields: object
    first, last = @first, @last
    step_size = step_size.to_f #people might not pass numbers in. This stops them.

    raise ArgumentError, "step can't be negative" if step_size < 0
    raise ArgumentError, "step can't be 0" if step_size == 0
    
    if step_size == 1
      each(&block)
    elsif Numeric === first
      cmp_method = self.exclude_end? ? :< : :<=
      
      while first.__send__(cmp_method, last)
        block.call(first)
        first += step_size
      end
    else
      counter = 0
      each do |o|
        block.call(o) if counter % step_size == 0
        counter += 1
      end
    end
    
    return self
  end

  # Convert this range object to a printable form.
  def to_s
    "#@first#{@exclude_end ? "..." : ".."}#@last"
  end
end

