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

end