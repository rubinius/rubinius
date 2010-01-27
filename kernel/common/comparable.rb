##
# The Comparable mixin is used by classes whose objects may be ordered. The
# class must define the <tt><=></tt> (spaceship) operator, which compares the
# receiver against another object, returning -1, 0, or +1 depending on whether
# the receiver is less than, equal to, or greater than the other object.
#
# Comparable uses <tt><=></tt> to implement the conventional comparison
# operators (<tt><</tt>, <tt><=</tt>, <tt>==</tt>, <tt>>=</tt>, and
# <tt>></tt>) and the method <tt>between?</tt>.
#
#   class SizeMatters
#     include Comparable
#     attr :str
#     def <=>(other)
#       str.size <=> other.str.size
#     end
#     def initialize(str)
#       @str = str
#     end
#     def inspect
#       @str
#     end
#   end
#   
#   s1 = SizeMatters.new "Z"
#   s2 = SizeMatters.new "YY"
#   s3 = SizeMatters.new "XXX"
#   s4 = SizeMatters.new "WWWW"
#   s5 = SizeMatters.new "VVVVV"
#   
#   s1 < s2                       #=> true
#   s4.between? s1, s3            #=> false
#   s4.between? s3, s5            #=> true
#   [ s3, s2, s5, s4, s1 ].sort   #=> [Z, YY, XXX, WWWW, VVVVV]

module Comparable

  # Compares two objects based on the receiver's <code><=></code>
  # method, returning true if it returns 0. Also returns true if
  # _obj_ and _other_ are the same object.
  def ==(other)
    return true if self.equal?(other)
    
    return unless c = (self <=> other)
    return c == 0 if c.is_a?(Numeric)

    raise ArgumentError, "comparison of #{self.class} with #{other.class} failed"
  rescue StandardError
    return nil
  end

  #  Compares two objects based on the receiver's <code><=></code>
  #  method, returning true if it returns 1.
  def >(other)
    __internal_compare__(other) > 0
  end

  #  Compares two objects based on the receiver's <code><=></code>
  #  method, returning true if it returns 0 or 1.
  def >=(other)
    __internal_compare__(other) >= 0
  end

  #  Compares two objects based on the receiver's <code><=></code>
  #  method, returning true if it returns -1.
  def <(other)
    __internal_compare__(other) < 0
  end

  #  Compares two objects based on the receiver's <code><=></code>
  #  method, returning true if it returns -1 or 0.
  def <=(other)
    __internal_compare__(other) <= 0
  end
  
  #  Returns <code>false</code> if <i>obj</i> <code><=></code>
  #  <i>min</i> is less than zero or if <i>anObject</i> <code><=></code>
  #  <i>max</i> is greater than zero, <code>true</code> otherwise.
  #     
  #     3.between?(1, 5)               #=> true
  #     6.between?(1, 5)               #=> false
  #     'cat'.between?('ant', 'dog')   #=> true
  #     'gnu'.between?('ant', 'dog')   #=> false
  #     
  def between?(min, max)
    (min <= self) && (self <= max)
  end
  
  # TODO: mark as internal only
  #private
  def __internal_compare__(other)
    unless result = (self <=> other)
      raise ArgumentError, "comparison of #{self.class} with #{other.class} failed"
    end
    
    return result
  end

  # A version of MRI's rb_cmpint (sort of)
  def self.compare_int(int)
    return int if int.kind_of? Fixnum

    return 1  if int > 0
    return -1 if int < 0
    return 0
  end
end

