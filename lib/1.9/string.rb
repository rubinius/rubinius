class String
  def self.try_convert(obj)
    return nil unless obj.respond_to?(:to_str)
    Type.coerce_to(obj, String, :to_str)
  end

  # Same as 1.8.7, but pattern is mandatory
  def partition(pattern)
    pattern = Type.coerce_to(pattern, String, :to_str) unless pattern.is_a? Regexp
    i = index(pattern)
    return [self, "", ""] unless i

    if pattern.is_a? Regexp
      match = Regexp.last_match
      [match.pre_match, match[0], match.post_match]
    else
      last = i+pattern.length
      [self[0...i], self[i...last], self[last...length]]
    end
  end

  def upto(stop, exclusive=false)
    return to_enum :upto, stop, exclusive unless block_given?
    stop = StringValue(stop)
    return self if self > stop

    if stop.size == 1 && size == 1
      after_stop = stop[0] + (exclusive ? 0 : 1)
      current = self[0]
      until current == after_stop
        yield current.chr
        current += 1
      end
    else
      after_stop = exclusive ? stop : stop.succ
      current = self

      until current == after_stop
        yield current
        current = StringValue(current.succ)
        break if current.size > stop.size || current.size == 0
      end
    end
    self
  end

end