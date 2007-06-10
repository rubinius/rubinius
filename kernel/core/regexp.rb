class Regexp

  ValidOptions = ['m','i','x']

  def self.last_match(field = nil)
    match = $~
    if match
      return match if field.nil?
      return match[field]
    else
      return nil
    end
  end

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

  def ~
    line = $_
    if !line.is_a?(String)
      $~ = nil
      return nil
    end
    res = self.match(line)
    return res.nil? ? nil : res.begin(0)
  end

  def =~(str)
    m = match(str)
    return m.begin(0) if m
    nil
  end

  def ===(other)
    if !other.is_a?(String)
      if !other.respond_to(:to_str)
        $~ = nil
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

  def match(str)
    obj = __regexp_match__(str)
    $~ = obj
    return obj
  end

  def __regexp_match__(str)
    Ruby.primitive :regexp_match
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

  ivar_as_index :__ivars__ => 0, :source => 1, :regexp => 2, :full => 3, :region => 4

  def [](idx, len = nil)
    if len
      return to_a[idx, len]
    elsif idx.is_a?(Symbol)
      num = @regexp.names[idx]
      raise ArgumentError, "Unknown named group '#{idx}'" unless num
      return get_capture(num)
    elsif !idx.is_a?(Integer) or idx < 0
      return to_a[idx]
    end
    
    if idx == 0
      return matched_area()
    else
      return get_capture(idx - 1)
    end    
  end

  def to_s
    matched_area()
  end

  def inspect
    "#<MatchData:0x#{object_id.to_s(16)} \"#{matched_area}\">"
  end

  def select
    unless block_given?
      raise LocalJumpError, "no block given"
    end
    
    out = []
    ma = matched_area()
    out << ma if yield ma
    
    each_capture do |str|
      if yield(str)
        out << str
      end
    end
    return out
  end

  alias_method :size, :length

  def to_a
    ary = captures()
    ary.unshift matched_area()
    return ary
  end

  def values_at(*indexes)
    indexes.map { |i| self[i] }
  end
  
  private
  
  def matched_area
    x = full[0]
    y = full[1]
    @source[x...y]
  end
  
  def get_capture(num)
    x, y = @region[num]
    return nil if !y or x == -1
    
    return @source[x...y]
  end
  
  def each_capture
    @region.each do |tup|
      x, y = *tup
      yield @source[x...y]
    end
  end

end

