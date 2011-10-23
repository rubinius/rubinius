class Regexp
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Regexp, :to_regexp
  end

  def ===(other)
    if other.kind_of? Symbol
      other = other.to_s
    elsif !other.kind_of? String
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

  def encoding
    source.encoding
  end

  def name_table
    @names
  end

  #
  # call-seq:
  #    rxp.named_captures  => hash
  #
  # Returns a hash representing information about named captures of <i>rxp</i>.
  #
  # A key of the hash is a name of the named captures.
  # A value of the hash is an array which is list of indexes of corresponding
  # named captures.
  #
  #    /(?<foo>.)(?<bar>.)/.named_captures
  #    #=> {"foo"=>[1], "bar"=>[2]}
  #
  #    /(?<foo>.)(?<foo>.)/.named_captures
  #    #=> {"foo"=>[1, 2]}
  #
  # If there are no named captures, an empty hash is returned.
  #
  #    /(.)(.)/.named_captures
  #    #=> {}
  #
  def named_captures
    hash = {}

    if @names
      @names.sort_by { |a,b| b.first }.each do |k, v| # LookupTable is unordered
        hash[k.to_s] = v
      end
    end

    return hash
  end

  #
  # call-seq:
  #    rxp.names   => [name1, name2, ...]
  #
  # Returns a list of names of captures as an array of strings.
  #
  #     /(?<foo>.)(?<bar>.)(?<baz>.)/.names
  #     #=> ["foo", "bar", "baz"]
  #
  #     /(?<foo>.)(?<foo>.)/.names
  #     #=> ["foo"]
  #
  #     /(.)(.)/.names
  #     #=> []
  #
  def names
    if @names
      @names.sort_by { |a,b| b.first }.map { |x| x.first.to_s } # LookupTable is unordered
    else
      []
    end
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
        return @source.substring(x, y-x)
      end
    when String
      if @regexp.name_table && num = @regexp.name_table[idx.to_sym]
        return self[num.last]
      else
        raise IndexError, "Unknown named group '#{idx}'"
      end
    when Symbol
      if @regexp.name_table && num = @regexp.name_table[idx]
        return self[num.last]
      else
        raise IndexError, "Unknown named group '#{idx}'"
      end
    end

    return to_a[idx]
  end

  attr_reader :regexp

  def ==(other)
    other.kind_of?(MatchData) &&
      string == other.string  &&
      regexp == other.regexp  &&
      captures == other.captures
  end
  alias_method :eql?, :==
end
