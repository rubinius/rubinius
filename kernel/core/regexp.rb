class Regexp

  ValidOptions = ['m','i','x']

  def self.union(*patterns)
    if patterns.nil? || patterns.length == 0
      return '/(?!)/'
    else
      flag  = false
      string = '/'
      patterns.each do |pattern|
        string += '|' if flag
        string += pattern.to_s
        flag = true
      end
      return string += '/'
    end
  end

  def ===(other)
    if !other.is_a?(String)
      if !other.respond_to(:to_str)
        #$~ = nil # FIXME should special global $~ (last_match)
        return false
      end
    end
    return !self.match(other.to_str).nil?
  end

  def casefold?
    (options & IGNORECASE) > 0 ? true : false
  end

  def eql?(other)
    return false unless Regexp === other
    # Ruby 1.8 doesn't destinguish between KCODE_NONE (16) & not specified (0) for eql?
    self_options  = options       & KCODE_MASK != 0 ? options       : options       + KCODE_NONE
    other_options = other.options & KCODE_MASK != 0 ? other.options : other.options + KCODE_NONE
    return (source == other.source) && ( self_options == other_options)
  end

  alias_method :==, :eql?

  def hash
    str = '/' << source << '/' << option_to_string(options)
    if options & KCODE_MASK == 0
      str << 'n'
    else
      str << kcode[0,1]
    end
    str.hash
  end

  def inspect
    '/' << source << '/' << option_to_string(options) << kcode[0,1]
  end

  def kcode
    lang = options & KCODE_MASK
    return "none" if lang == KCODE_NONE
    return "euc"  if lang == KCODE_EUC
    return 'sjis' if lang == KCODE_SJIS
    return 'utf8' if lang == KCODE_UTF8
    return nil
  end

  def options
    Ruby.primitive :regexp_options
  end


  def to_s
    idx     = 0
    offset  = 0
    pattern = source
    option  = options
    len     = pattern.size
    endpt   = -1

    loop do
      if (len - idx) > 4 && pattern[idx,2] == "(?"
        idx += 2

        offset = get_option_string_length(pattern[idx..-1])
        if offset > 0
          option |= string_to_option(pattern[idx, offset])
          idx += offset
        end

        if pattern[idx,1] == '-'
          idx += 1
          offset = get_option_string_length(pattern[idx..-1])
          if offset > 0
            option &= ~string_to_option(pattern[idx, offset])
            idx += offset
          end
        end

        if pattern[idx..1] == ')'
          idx += 1
          next
        elsif pattern[idx,1] == ':' && pattern[-1,1] == ')'
          idx += 1
          if !Regexp.new(pattern[idx..-2], 0).is_a?(Regexp)
            option = self.options
            idx    = 0
          else
            endpt -= 1
          end
        end
      end
      break
    end
    string = '(?'
    string << option_to_string(option)
    if (option & OPTION_MASK) != OPTION_MASK
      string << '-' << option_to_string(~option)
    end
    string << ':' << pattern[idx..endpt] << ')'
  end

  # private functions
  def get_option_string_length(string)
    idx = 0
    while idx < string.length do
      if !ValidOptions.include?(string[idx,1])
        return idx
      end
      idx += 1
    end
    return idx
  end

  def option_to_string(option)
    string = ""
    string << 'm' if (option & MULTILINE) > 0
    string << 'i' if (option & IGNORECASE) > 0
    string << 'x' if (option & EXTENDED) > 0
    return string
  end

end

class MatchData

  def [](idx, len = nil)
    if idx.is_a?(Integer)
      idx += self.length if idx < 0
      if len.nil?
        if idx >= 0 && idx <= self.length
          x = self.begin(idx)
          y = self.end(idx)
          return @source[x...y]
        else
          return nil
        end
      else
        if idx >=0 && idx < self.length
          return get_match_array(0)[idx, len]
        else
          return nil
        end
      end
    elsif idx.respond_to?(:first) && idx.respond_to?(:last)
      return get_match_array(0)[idx]
    end
    return nil
  end

  def captures
    return get_match_array(1)
  end

  def inspect
    self[0]
  end

  def select(*index)
    out = Array.new
    if index.is_a?(Array)
      index.each do |idx|
        out << self[idx]
      end
    end
    return out
  end

  alias_method :size, :length

  def to_a
    return get_match_array(0)
  end

  alias_method :to_s, :inspect

  # private functions
  def get_match_array(start)
    out  = Array.new
    last = self.length
    (start...last).each do |i|
      x = self.begin(i)
      y = self.end(i)
      out << @source[x...y]
    end
    return out
  end

end

