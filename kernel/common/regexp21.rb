# -*- encoding: us-ascii -*-

class Regexp
  FIXEDENCODING = 16
  NOENCODING    = 32

  OPTION_MASK = IGNORECASE | EXTENDED | MULTILINE | FIXEDENCODING | NOENCODING | DONT_CAPTURE_GROUP | CAPTURE_GROUP

  def initialize(pattern, opts=nil, lang=nil)
    if pattern.kind_of?(Regexp)
      opts = pattern.options
      pattern = pattern.source
    elsif opts.kind_of?(Fixnum)
      opts = opts & (OPTION_MASK | KCODE_MASK) if opts > 0
    elsif opts
      opts = IGNORECASE
    else
      opts = 0
    end

    opts |= NOENCODING if lang == 'n'

    compile pattern, opts
  end

  def initialize_copy(other)
    initialize other.source, other.options
  end

  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Regexp, :to_regexp
  end

  def match(str, pos=0)
    unless str
      Regexp.last_match = nil
      return nil
    end

    str = str.to_s if str.is_a?(Symbol)
    str = StringValue(str)

    m = Rubinius::Mirror.reflect str
    pos = pos < 0 ? pos + str.size : pos
    pos = m.character_to_byte_index pos
    result = search_region(str, pos, str.bytesize, true)
    Regexp.last_match = result

    if result && block_given?
      yield result
    else
      result
    end
  end

  # Returns the index of the first character in the region that
  # matched or nil if there was no match. See #match for returning
  # the MatchData instead.
  def =~(str)
    str = str.to_s if str.is_a?(Symbol)
    # unless str.nil? because it's nil and only nil, not false.
    str = StringValue(str) unless str.nil?

    match = match_from(str, 0)
    if match
      Regexp.last_match = match
      return match.begin(0)
    else
      Regexp.last_match = nil
      return nil
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

  def eql?(other)
    return false unless other.kind_of?(Regexp)
    return false unless source == other.source
    (options & ~NOENCODING) == (other.options & ~NOENCODING)
  end

  alias_method :==, :eql?

  def hash
    str = '/' << source << '/' << option_to_string(options)
    str.hash
  end

  def inspect
    # the regexp matches any / that is after anything except for a \
    escape = source.gsub(%r!(\\.)|/!) { $1 || '\/' }
    str = "/#{escape}/#{option_to_string(options)}"
    str << 'n' if (options & NOENCODING) > 0
    str
  end

  def encoding
    source.encoding
  end

  def self.union(*patterns)
    case patterns.size
    when 0
      return %r/(?!)/
    when 1
      pat = patterns.first
      case pat
      when Array
        return union(*pat)
      when Regexp
        return pat
      else
        return Regexp.new(Regexp.quote(StringValue(pat)))
      end
    end

    str = ""
    patterns.each_with_index do |pat, idx|
      str << "|" if idx != 0

      if pat.kind_of? Regexp
        str << pat.to_s
      else
        str << Regexp.quote(StringValue(pat))
      end
    end

    Regexp.new(str)
  end

  def self.escape(str)
    str = str.to_s if str.is_a?(Symbol)
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

  def begin(idx)
    if idx == 0
      start = @full.at(0)
    else
      start = @region.at(idx - 1).at(0)
      return nil if start == -1
    end
    m = Rubinius::Mirror.reflect @source
    m.byte_to_character_index start
  end

  def end(idx)
    if idx == 0
      fin = @full.at(1)
    else
      fin = @region.at(idx - 1).at(1)
      return nil if fin == -1
    end
    m = Rubinius::Mirror.reflect @source
    m.byte_to_character_index fin
  end

  def offset(idx)
    out = []
    out << self.begin(idx)
    out << self.end(idx)
    return out
  end

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
