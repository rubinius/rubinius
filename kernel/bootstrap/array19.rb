class Array
  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    return to_enum :map unless block_given?
    array = Array.new size
    i = -1
    each { |x| array[i+=1] = yield(x) }
    array
  end

  def self.coerce_into_array(obj)
    return [obj] unless obj

    return obj.to_a   if obj.respond_to?(:to_a)
    return obj.to_ary if obj.respond_to?(:to_ary)

    # On 1.9, #to_a is not defined on all objects, so wrap the object in a
    # literal array.
    return [obj]
  end
end
