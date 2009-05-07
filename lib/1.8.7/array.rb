class Array
  # When invoked with a block, yields all combinations of length n of elements
  # from ary and then returns ary itself. Even though the implementation makes
  # no guarantees about the order in which the combinations are yielded, we copy MRI.
  # When invoked without a block, returns an enumerator object instead.
  def combination(num)
    num = Type.coerce_to num, Fixnum, :to_int
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
  
  # Calls block for each element repeatedly n times or forever if none
  # or nil is given. If a non-positive number is given or the array is empty,
  # does nothing. Returns nil if the loop has finished without getting interrupted.
  def cycle(n = nil, &block)
    return to_enum(:cycle, n) unless block_given?
    if n.nil?
      loop(&block)
    else
      n = Type.coerce_to n, Fixnum, :to_int
      n.times{each(&block)}
    end
    nil
  end

  # Recursively flatten any contained Arrays into an one-dimensional result.
  # The optional level argument determines the level of recursion to flatten
  def flatten(level=Undefined)
    dup.flatten!(level) || self
  end

  # Flattens self in place as #flatten. If no changes are
  # made, returns nil, otherwise self.
  # The optional level argument determines the level of recursion to flatten
  def flatten!(level=Undefined)
    if level.equal? Undefined
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
    if obj.equal? Undefined
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

  # Removes and returns the last element from the Array.
  # If a number n is given, returns an array of the last n elements (or less)
  # just like array.slice!(-n, n) does.
  alias_method :single_pop, :pop
  private :single_pop
  def pop(n = Undefined)
    if n.equal? Undefined
      single_pop
    else
      n = Type.coerce_to(n, Fixnum, :to_int)
      raise ArgumentError, "negative array size" if n < 0
      first = size - n
      first = 0 if first < 0
      slice!(first..size).to_a
    end
  end

  # Returns an array of all combinations of elements from all arrays.
  # The length of the returned array is the product of the length of
  # ary and the argument arrays
  def product(*arg)
    # Implementation notes: We build an enumerator for all the combinations
    # by building it up successively using "inject" and starting from a trivial enumerator.
    # It would be easy to have "product" yield to a block but the standard
    # simply returns an array, so you'll find a simple call to "to_a" at the end.
    #
    trivial_enum = Enumerator.new_with_block{|yielder| yielder.yield [] }
    [self, *arg].map{|x| Type.coerce_to(x, Array, :to_ary)}.
      inject(trivial_enum) do |enum, array|
        Enumerator.new_with_block do |yielder|
          enum.each do |partial_product|
            array.each do |obj|
              yielder.yield partial_product + [obj]
            end
          end
        end
    end.to_a
  end unless method_defined? :product

  # Returns the index of the last element in the Array
  # for which elem == obj is true. If a block is given
  # instead of an argument, returns last object for
  # which block is true
  def rindex(obj = Undefined)
    i = @total - 1
    if obj.equal? Undefined
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
  
  # Choose a random element, or the random n elements, from the array.
  # If the array is empty, the first form returns nil, and the second
  # form returns an empty array.
  # Note: Actually available in 1.9 only; 1.8.7 has #choice
  def sample(n = Undefined)
    return at(rand(size)) if n.equal? Undefined
    n = Type.coerce_to(n, Fixnum, :to_int)
    raise ArgumentError, "negative array size" if n < 0
    n = size if n > size
    result = Array.new(self)
    n.times do |i|
      r = i + rand(size - i)
      result.tuple.swap(i,r)
    end
    result[n..size] = []
    result
  end
  
  # Removes and returns the first element from the Array.
  # If a number n is given, returns an array of the first n elements (or less)
  # just like array.slice!(0, n) does.
  alias_method :single_shift, :shift
  private :single_shift
  def shift(n = Undefined)
    if n.equal? Undefined
      single_shift
    else
      n = Type.coerce_to(n, Fixnum, :to_int)
      raise ArgumentError, "negative array size" if n < 0
      slice!(0, n).to_a
    end
  end

  # Returns a new array with elements of this array shuffled. 
  def shuffle
    Array.new(self).shuffle!
  end

  # Shuffles elements in self in place. 
  def shuffle!
    size.times do |i|
      r = i + rand(size - i)
      @tuple.swap(i,r)
    end
    self
  end

  # Deletes the element(s) given by an index (optionally with a length)
  # or by a range. Returns the deleted object, subarray, or nil if the
  # index is out of range. Equivalent to:
  def slice!(*args)
    out = self[*args]
    if !(Range === args[0])
      # make sure that negative values are not passed through to the
      # []= assignment
      args[0] = Type.coerce_to args[0], Integer, :to_int
      args[0] += self.length if args[0] < 0
      # Don't extend the array for out of bounds ranges
      return out if args[0] < 0 || args[0] > self.length
      args << 1 if args.size == 1
    else
      # Don't extend the array for out of bounds ranges
      return out if args[0].begin.to_int > self.length
    end
    self[*args] = []
    out
  end
  
end
