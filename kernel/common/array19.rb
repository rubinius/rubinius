class Array
  # Try to convert obj into an array, using to_ary method.
  # Returns converted array or nil if obj cannot be converted
  # for any reason. This method is to check if an argument is an array.
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Array, :to_ary
  end

  # Choose a random element, or the random n elements, from the array.
  # If the array is empty, the first form returns nil, and the second
  # form returns an empty array.
  def sample(n=undefined)
    return at(Kernel.rand(size)) if n.equal? undefined

    n = Rubinius::Type.coerce_to(n, Fixnum, :to_int)
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

  alias_method :to_s, :inspect

  def uniq
    dup.uniq! or dup
  end

  def uniq!
    Rubinius.check_frozen

    im = Rubinius::IdentityMap.from self
    return if im.size == size

    array = im.to_array
    @tuple = array.tuple
    @start = array.start
    @total = array.total

    self
  end
end
