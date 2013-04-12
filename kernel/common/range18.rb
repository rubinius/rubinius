# -*- encoding: us-ascii -*-

class Range
  alias_method :===, :include?

  def each(&block)
    return to_enum unless block_given?
    first, last = @begin, @end

    raise TypeError, "can't iterate from #{first.class}" unless can_iterate_from?(first)

    case first
    when Fixnum
      last -= 1 if @excl

      i = first
      while i <= last
        yield i
        i += 1
      end

    when String
      first.upto(last, @excl, &block)
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

  alias_method :first, :begin
  alias_method :last, :end

  def can_iterate_from?(object)
    object.respond_to? :succ
  end
  private :can_iterate_from
end
