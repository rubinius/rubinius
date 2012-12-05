# -*- encoding: us-ascii -*-

class Array
  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    return dup unless block_given?
    tup = Rubinius::Tuple.new size

    i = @start
    total = i + @total
    tuple = @tuple

    j = 0
    while i < total
      tup[j] = yield tuple.at(i)
      i += 1
      j += 1
    end

    Array.wrap tup, size
  end
end
