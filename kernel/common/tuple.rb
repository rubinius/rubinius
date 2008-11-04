# depends on: class.rb module.rb

##
# The tuple data type.
# A simple storage class. Created to contain a fixed number of elements.
#
# Not designed to be subclassed, as it does not call initialize
# on new instances.


class Tuple

  include Enumerable

  def self.[](*args)
    sz = args.size
    tup = new(sz)
    i = 0
    while i < sz
      tup.put i, args[i]
      i += 1
    end

    return tup
  end

  def to_s
    "#<Tuple:0x#{object_id.to_s(16)} #{fields} elements>"
  end

  def each
    i = 0
    t = fields
    while i < t
      yield self.at(i)
      i += 1
    end
    self
  end

  def + o
    t = Tuple.new(size + o.size)
    t.copy_from(self,0,0)
    t.copy_from(o,0,size)
    t
  end

  def inspect
    str = "#<Tuple"
    if fields == 0
      str << " empty>"
    else
      str << ": #{join(", ", :inspect)}>"
    end
    return str
  end

  def join(sep, meth=:to_s)
    join_upto(sep, fields, meth)
  end

  def join_upto(sep, count, meth=:to_s)
    str = ""
    return str if count == 0 or empty?
    count = fields if count >= fields
    count -= 1
    i = 0
    while i < count
      str.append at(i).__send__(meth)
      str.append sep.dup
      i += 1
    end
    str.append at(count).__send__(meth)
    return str
  end

  def ===(other)
    return false unless Tuple === other and fields == other.fields
    i = 0
    while i < fields
      return false unless at(i) === other.at(i)
      i += 1
    end
    true
  end

  def to_a
    ary = []
    ary.tuple = dup
    ary.total = fields
    ary.start = 0
    return ary
  end

  def shift
    return self unless fields > 0
    t = Tuple.new(fields-1)
    t.copy_from self, 1, 0
    return t
  end

  # Swap elements of the two indexes.
  def swap(a, b)
    temp = at(a)
    put(a, at(b))
    put(b, temp)
  end

  def enlarge(size)
    if size > fields()
      t = Tuple.new(size)
      t.copy_from self, 0, 0
      return t
    end

    return self
  end

  alias_method :size, :fields
  alias_method :length, :fields

  def empty?
    size == 0
  end

  def first
    at(0)
  end

  def last
    at(fields-1)
  end
end
