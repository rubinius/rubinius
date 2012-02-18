# -*- encoding: us-ascii -*-

class Range
  ##
  # :call-seq:
  #   rng.each { |i| block }  => rng
  #
  # Iterates over the elements +rng+, passing each in turn to the block. You
  # can only iterate if the start object of the range supports the
  # succ method (which means that you can't iterate over ranges of
  # Float objects).
  #
  #   (10..15).each do |n|
  #      print n, ' '
  #   end
  #
  # produces:
  #
  #   10 11 12 13 14 15

  def each(&block)
    return to_enum unless block_given?
    first, last = @begin, @end

    raise TypeError, "can't iterate from #{first.class}" unless first.respond_to? :succ

    case first
    when Fixnum
      last -= 1 if @excl

      i = first
      while i <= last
        yield i
        i += 1
      end

    when String
      first.upto(last) do |s|
        yield s unless @excl && s == last
      end
    when Symbol
      current = first
      if @excl
        while (current <=> last) < 0
          yield current
          current = (current.to_s.bytes.to_a.last + 1).chr.to_sym
        end
      else
        while (c = current <=> last) && c <= 0
          yield current
          break if c == 0
          current = (current.to_s.bytes.to_a.last + 1).chr.to_sym
        end
      end
    else
      current = first
      if @excl
        while (current <=> last) < 0
          yield current
          current = current.succ
        end
      else
        while (c = current <=> last) && c <= 0
          yield current
          break if c == 0
          current = current.succ
        end
      end
    end
    return self
  end
end