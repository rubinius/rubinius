# -*- encoding: us-ascii -*-

class Range
  def ===(value)
    include?(value)
  end

  alias_method :cover?, :include?

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
