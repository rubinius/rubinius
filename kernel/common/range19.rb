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
end
