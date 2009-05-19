class Array

  class << self
    # Try to convert obj into an array, using to_ary method.
    # Returns converted array or nil if obj cannot be converted
    # for any reason. This method is to check if an argument is an array.
    def try_convert(obj)
      return nil unless obj.respond_to?(:to_ary)
      Type.coerce_to(obj, Array, :to_ary)
    end
  end

  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    return to_enum :map unless block_given?
    return dup unless block_given?
    out = Array.new @total
    i = 0
    while i < @total
      out[i] = yield(at(i))
      i += 1
    end
    out
  end

  public :sample
  remove_method :choice
end