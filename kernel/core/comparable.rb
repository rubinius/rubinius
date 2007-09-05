#  The <code>Comparable</code> mixin is used by classes whose objects
#  may be ordered. The class must define the <code><=></code> operator,
#  which compares the receiver against another object, returning -1, 0,
#  or +1 depending on whether the receiver is less than, equal to, or
#  greater than the other object. <code>Comparable</code> uses
#  <code><=></code> to implement the conventional comparison operators
#  (<code><</code>, <code><=</code>, <code>==</code>, <code>>=</code>,
#  and <code>></code>) and the method <code>between?</code>.
#     
#     class SizeMatters
#       include Comparable
#       attr :str
#       def <=>(anOther)
#         str.size <=> anOther.str.size
#       end
#       def initialize(str)
#         @str = str
#       end
#       def inspect
#         @str
#       end
#     end
#     
#     s1 = SizeMatters.new("Z")
#     s2 = SizeMatters.new("YY")
#     s3 = SizeMatters.new("XXX")
#     s4 = SizeMatters.new("WWWW")
#     s5 = SizeMatters.new("VVVVV")
#     
#     s1 < s2                       #=> true
#     s4.between?(s1, s3)           #=> false
#     s4.between?(s3, s5)           #=> true
#     [ s3, s2, s5, s4, s1 ].sort   #=> [Z, YY, XXX, WWWW, VVVVV]
#     
module Comparable
  #  :call-seq:
  #     obj == other    => true or false
  #  
  #  Compares two objects based on the receiver's <code><=></code>
  #  method, returning true if it returns 0. Also returns true if
  #  _obj_ and _other_ are the same object.

  def ==(other)
    self.object_id == other.object_id || (self <=> other) == 0
  end

  #  :call-seq:
  #     obj > other    => true or false
  #  
  #  Compares two objects based on the receiver's <code><=></code>
  #  method, returning true if it returns 1.
  def >(other)
    (self <=> other) > 0
  end

  #  :call-seq:
  #     obj >= other    => true or false
  #  
  #  Compares two objects based on the receiver's <code><=></code>
  #  method, returning true if it returns 0 or 1.
  def >=(other)
    (self <=> other) >= 0
  end

  #  :call-seq:
  #     obj < other    => true or false
  #  
  #  Compares two objects based on the receiver's <code><=></code>
  #  method, returning true if it returns -1.
  def <(other)
    (self <=> other) < 0
  end

  #  :call-seq:
  #     obj <= other    => true or false
  #  
  #  Compares two objects based on the receiver's <code><=></code>
  #  method, returning true if it returns -1 or 0.
  def <=(other)
    (self <=> other) <= 0
  end
  
  #  :call-seq:
  #     obj.between?(min, max)    => true or false
  #  
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
end

