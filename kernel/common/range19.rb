# -*- encoding: us-ascii -*-

class Range
  def ===(value)
    include?(value)
  end

  alias_method :cover?, :include?

  def each(&block)
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
      first.upto(last, @excl, &block)
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
    n.equal?(undefined) ? @begin : super
  end

  def max(&block)
    return super(&block) if block_given? || (@excl && !@end.kind_of?(Numeric))
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

  def min(&block)
    return super(&block) if block_given?
    return nil if @end < @begin || (@excl && @end == @begin)
    @begin
  end

  protected

  def can_iterate_from?(object)
    first.respond_to?(:succ) && !object.kind_of?(Time)
  end
end
