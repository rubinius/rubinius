class Array
  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    return to_enum :map unless block_given?
    out = Array.new size

    i = @start
    total = i + @total
    tuple = @tuple

    out_tuple = out.tuple

    j = 0
    while i < total
      out_tuple[j] = yield tuple.at(i)
      i += 1
      j += 1
    end

    out
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
