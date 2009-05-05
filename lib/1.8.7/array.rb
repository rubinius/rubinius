class Array
  # Recursively flatten any contained Arrays into an one-dimensional result.
  # The optional level argument determines the level of recursion to flatten
  def flatten(level=Undefined)
    dup.flatten!(level) || self
  end

  # Flattens self in place as #flatten. If no changes are
  # made, returns nil, otherwise self.
  # The optional level argument determines the level of recursion to flatten
  def flatten!(level=Undefined)
    if level == Undefined
      level = -1
    else
      level = Type.coerce_to(level, Integer, :to_int)
    end
    ret, out = nil, []
    ret = recursively_flatten(self, out, level)
    replace(out) if ret
    ret
  end
  
  # Returns the index of the first element in the Array
  # for which elem == obj is true or nil. If a block is
  # given instead of an argument, returns first object
  # for which block is true. Returns nil if no match is found.
  def index(obj = Undefined)
    i = 0
    if obj == Undefined
      while i < @total do
        return i if yield at(i)
        i += 1
      end
    else
      while i < @total do
        return i if at(i) == obj
        i += 1
      end
    end
    nil
  end

  # Returns the index of the last element in the Array
  # for which elem == obj is true. If a block is given
  # instead of an argument, returns last object for
  # which block is true
  def rindex(obj = Undefined)
    i = @total - 1
    if obj == Undefined
      while i >= 0 do
        return i if yield at(i)
        i -= 1
      end
    else
      while i >= 0 do
        return i if at(i) == obj
        i -= 1
      end
    end
    nil
  end
  
end
