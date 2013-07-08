# -*- encoding: us-ascii -*-

class Range
  include Enumerable

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

  def ==(other)
    return true if equal? other

    other.kind_of?(Range) and
      self.first == other.first and
      self.last == other.last and
      self.exclude_end? == other.exclude_end?

  end
  alias_method :eql?, :==

  attr_reader_specific :excl, :exclude_end?

  attr_reader :begin

  def hash
    excl = @excl ? 1 : 0
    hash = excl
    hash ^= @begin.hash << 1
    hash ^= @end.hash << 9
    hash ^= excl << 24;
    # Are we throwing away too much here for a good hash value distribution?
    return hash & Fixnum::MAX
  end

  def inspect
    "#{@begin.inspect}#{@excl ? "..." : ".."}#{@end.inspect}"
  end

  attr_reader :end

  def step(step_size=1) # :yields: object
    return to_enum(:step, step_size) unless block_given?
    first = @begin
    last = @end

    if step_size.kind_of? Float or first.kind_of? Float or last.kind_of? Float
      # if any are floats they all must be
      begin
        step_size = Float(from = step_size)
        first     = Float(from = first)
        last      = Float(from = last)
      rescue ArgumentError
        raise TypeError, "no implicit conversion to float from #{from.class}"
      end
    else
      step_size = Integer(from = step_size)
      if ! step_size.kind_of? Integer
        raise TypeError, "can't convert #{from.class} to Integer (#{from.class}#to_int gives #{step_size.class})"
      end
    end

    if step_size <= 0
      raise ArgumentError, "step can't be negative" if step_size < 0
      raise ArgumentError, "step can't be 0"
    end

    if first.kind_of?(Float)
      err = (first.abs + last.abs + (last - first).abs) / step_size.abs * Float::EPSILON
      err = 0.5 if err > 0.5
      if @excl
        n = ((last - first) / step_size - err).floor
        n += 1 if n * step_size + first < last
      else
        n = ((last - first) / step_size + err).floor + 1
      end

      i = 0
      while i < n
        d = i * step_size + first
        d = last if last < d
        yield d
        i += 1
      end
    elsif first.kind_of?(Numeric)
      d = first
      while @excl ? d < last : d <= last
        yield d
        d += step_size
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

  def to_s
    "#{@begin}#{@excl ? "..." : ".."}#{@end}"
  end

  def to_a
    if @begin.kind_of? Fixnum and @end.kind_of? Fixnum
      fin = @end
      fin += 1 unless @excl

      size = fin - @begin
      return [] if size <= 0

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

  def __cover__?(value)
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
  private :__cover__?
end

