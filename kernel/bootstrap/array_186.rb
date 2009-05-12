# These methods are overriden by lib/1.8.7/... or lib/1.9/...
class Array
  # Passes each element in the Array to the given block
  # and returns self.  We re-evaluate @total each time
  # through the loop in case the array has changed.
  def each
    i = 0
    while i < @total
      yield at(i)
      i += 1
    end
    self
  end

  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    return dup unless block_given?
    out = Array.new @total
    i = 0
    while i < @total
      out[i] = yield(at(i))
      i += 1
    end
    out
  end

  # Replaces each element in self with the return value
  # of passing that element to the supplied block.
  def map!
    i = 0
    while i < @total
      self[i] = yield(at(i))
      i += 1
    end
    self
  end


end