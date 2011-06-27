class Array
  # Choose a random element, or the random n elements, from the array.
  # If the array is empty, the first form returns nil, and the second
  # form returns an empty array.
  def choice(n=undefined)
    return at(Kernel.rand(size)) if n.equal? undefined

    n = Type.coerce_to(n, Fixnum, :to_int)
    raise ArgumentError, "negative array size" if n < 0

    n = size if n > size
    result = Array.new(self)

    n.times do |i|
      r = i + Kernel.rand(size - i)
      result.tuple.swap(i,r)
    end

    result[n..size] = []
    result
  end

  # Produces a string by joining all elements without a
  # separator. See #join
  def to_s
    join
  end

  # Returns a new Array by removing duplicate entries
  # from self. Equality is determined by using a Hash
  def uniq
    dup.uniq! or dup
  end

  # Removes duplicates from the Array in place as #uniq
  def uniq!
    im = Rubinius::IdentityMap.from self
    return if im.size == size

    Ruby.check_frozen

    array = im.to_array
    @tuple = array.tuple
    @start = array.start
    @total = array.total

    self
  end
end
