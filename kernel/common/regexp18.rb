# -*- encoding: us-ascii -*-

class Regexp
  def match(str)
    unless str
      Regexp.last_match = nil
      return nil
    end

    str = StringValue(str)

    Regexp.last_match = search_region(str, 0, str.bytesize, true)
  end

  def ===(other)
    unless other.kind_of?(String)
      other = Rubinius::Type.check_convert_type other, String, :to_str
      unless other
        Regexp.last_match = nil
        return false
      end
    end

    if match = match_from(other, 0)
      Regexp.last_match = match
      true
    else
      Regexp.last_match = nil
      false
    end
  end

  def self.escape(str)
    StringValue(str).transform(ESCAPE_TABLE, true)
  end

  class << self
    alias_method :quote, :escape
  end
end

class MatchData
  def [](idx, len = nil)
    return to_a[idx, len] if len

    case idx
    when Fixnum
      if idx <= 0
        return matched_area() if idx == 0
        return to_a[idx]
      elsif idx <= @region.size
        tup = @region[idx - 1]

        x = tup.at(0)
        return nil if x == -1

        y = tup.at(1)
        return @source.byteslice(x, y-x)
      end
    when Symbol
      num = @regexp.name_table[idx]
      raise ArgumentError, "Unknown named group '#{idx}'" unless num
      return self[num + 1]
    when String
      num = @regexp.name_table[idx.to_sym]
      raise ArgumentError, "Unknown named group '#{idx}'" unless num
      return self[num + 1]
    end

    return to_a[idx]
  end
end
