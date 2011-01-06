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
end
