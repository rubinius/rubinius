class Array
  # When invoked with a block, yields all combinations of length n of elements
  # from ary and then returns ary itself. Even though the implementation makes
  # no guarantees about the order in which the combinations are yielded, we copy MRI.
  # When invoked without a block, returns an enumerator object instead.
  def combination(num)
    num = num.to_i
    return to_enum(:combination, num) unless block_given?
    return self unless (0..size).include? num
    # Implementation note: slightly tricky.
                                             # Example: self = 1..7, num = 3
    picks = (0...num).to_a                   # picks start at 0, 1, 2
    max = ((size-num)...size).to_a           # max (index for a given pick) is [4, 5, 6]
    pick_max_pairs = picks.zip(max).reverse  # pick_max_pairs = [[2, 6], [1, 5], [0, 4]]
    lookup = pick_max_pairs.find(Proc.new{return self})
    loop do
      yield values_at(*picks)
      move = lookup.each{|pick, max| picks[pick] < max}.first
      new_index = picks[move] + 1
      picks[move...num] = (new_index...(new_index+num-move)).to_a
    end
  end unless method_defined? :combination

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
