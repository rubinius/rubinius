# -*- encoding: us-ascii -*-

class Array
  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map
    return to_enum :map unless block_given?
    out = Rubinius::Tuple.new size

    i = @start
    total = i + @total
    tuple = @tuple

    j = 0
    while i < total
      out[j] = yield tuple.at(i)
      i += 1
      j += 1
    end

    Array.wrap out, size
  end
end
