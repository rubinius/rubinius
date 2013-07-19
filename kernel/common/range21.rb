# -*- encoding: us-ascii -*-

class Range
  alias_method :cover?, :__cover__?
  public :cover?

  def include?(value)
    if @begin.respond_to?(:to_int) || @end.respond_to?(:to_int) || 
       @begin.kind_of?(Numeric) || @end.kind_of?(Numeric)
      __cover__? value
    else
      super
    end
  end
  alias_method :member?, :include?

  def ===(value)
    include?(value)
  end

  def each
    return to_enum unless block_given?
    first, last = @begin, @end

    raise TypeError, "can't iterate from #{first.class}" unless can_iterate_from?(first)

    case first
    when Fixnum
      last -= 1 if @excl

      i = first
      while i <= last
        yield i
        i += 1
      end

    when String
      first.upto(last, @excl) do |i|
        yield i
      end
    when Symbol
      first.to_s.upto(last.to_s, @excl) do |str|
        yield str.to_sym
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

  def first(n=undefined)
    undefined.equal?(n) ? @begin : super
  end

  def last(n=undefined)
    undefined.equal?(n) ? @end : to_a.last(n)
  end

  def max
    return super if block_given? || (@excl && !@end.kind_of?(Numeric))
    return nil if @end < @begin || (@excl && @end == @begin)

    if @excl
      unless @end.kind_of?(Integer)
        raise TypeError, "cannot exclude non Integer end value"
      end

      unless @begin.kind_of?(Integer)
        raise TypeError, "cannot exclude end value with non Integer begin value"
      end

      return @end - 1
    end

    @end
  end

  def min
    return super if block_given?
    return nil if @end < @begin || (@excl && @end == @begin)
    @begin
  end

  def can_iterate_from?(object)
    object.respond_to?(:succ) && !object.kind_of?(Time)
  end
  private :can_iterate_from?
end
