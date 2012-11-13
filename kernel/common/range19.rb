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
    raise TypeError, "cannot exclude non Integer end value" if @end.kind_of?(Float) && @excl

    return super(&block) if block_given?
    return nil if @end < @begin || (@excl && @end == @begin)
    return @end if @end.kind_of?(Float) || (!@end.kind_of?(Float) && !@excl)
    super
  end

  def min(&block)
    return super(&block) if block_given?
    return nil if @end < @begin || (@excl && @end == @begin)
    return @begin if @begin.kind_of?(Float)
    super
  end

  protected

  def can_iterate_from?(object)
    first.respond_to?(:succ) && !object.kind_of?(Time)
  end
end
