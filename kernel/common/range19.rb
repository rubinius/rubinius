# -*- encoding: us-ascii -*-

class Range
  alias_method :cover?, :===

  def max(&block)
    return super(&block) if block_given?
    return nil if @end < @begin || (@excl && @end == @begin)
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
