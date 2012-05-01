# -*- encoding: us-ascii -*-

class Regexp
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Regexp, :to_regexp
  end

  def match(str, pos=0)
    unless str
      Regexp.last_match = nil
      return nil
    end

    str = StringValue(str)
    result = search_region(str, pos, str.bytesize, true)
    Regexp.last_match = result

    if result && block_given?
      yield result
    else
      result
    end
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

  def self.escape(str)
    escaped = StringValue(str).transform(ESCAPE_TABLE, true)
    if escaped.ascii_only?
      escaped.force_encoding Encoding::US_ASCII
    elsif str.valid_encoding?
      escaped.force_encoding str.encoding
    else
      escaped.force_encoding Encoding::ASCII_8BIT
    end
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
    when String
      if @regexp.name_table
        num = @regexp.name_table[idx.to_sym]
        return self[num.last] if num
      end
      raise IndexError, "Unknown named group '#{idx}'"
    when Symbol
      if @regexp.name_table
        num = @regexp.name_table[idx]
        return self[num.last] if num
      end
      raise IndexError, "Unknown named group '#{idx}'"
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
