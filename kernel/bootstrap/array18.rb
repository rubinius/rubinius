class Array
  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    return dup unless block_given?
    array = Array.new size
    i = -1
    each { |x| array[i+=1] = yield(x) }
    array
  end

  def self.coerce_into_array(obj)
    return [obj] unless obj

    return obj.to_ary if obj.respond_to?(:to_ary)

    # Just call #to_a, which wraps the reciever in an
    # array if it's not one.
    return obj.to_a
  end
end
