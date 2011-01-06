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
end
