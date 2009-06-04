# These methods are overriden by lib/1.8.7/... or lib/1.9/...
class Array
  # Deletes every element from self for which block evaluates to true
  def delete_if(&block)
    key = Undefined
    i = @start
    tot = @start + @total
    while i < tot
      if yield(@tuple.at(i))
        @tuple.put(i, key)
      end
      i+=1
    end

    if (deleted = @tuple.delete(@start,@total,key)) > 0
      @total -= deleted
      reallocate_shrink()
    end
    return self
  end

  # Passes each index of the Array to the given block
  # and returns self.  We re-evaluate @total each time
  # through the loop in case the array has changed.
  def each_index
    i = 0
    while i < @total
      yield i
      i += 1
    end
    self
  end

  # Returns a new Array by removing items from self for
  # which block is true. An Array is also returned when
  # invoked on subclasses. See #reject!
  def reject(&block)
    Array.new(self).reject!(&block) || self
  end

  # Equivalent to #delete_if except that returns nil if
  # no changes were made.
  def reject!(&block)
    was = length
    self.delete_if(&block)

    self if was != length     # Too clever?
  end

  # Goes through the Array back to front and yields
  # each element to the supplied block. Returns self.
  def reverse_each
    i = @total - 1
    while i >= 0 do
      yield at(i)
      i = @total if @total < i
      i -= 1
    end
    self
  end

end
