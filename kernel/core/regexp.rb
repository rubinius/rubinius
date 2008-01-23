# depends on: class.rb string.rb

class Regexp

  ivar_as_index :__ivars__ => 0, :source => 1, :data => 2, :names => 3
  def __ivars__; @__ivars__ ; end
  def source   ; @source    ; end
  def data     ; @data      ; end
  def names    ; @names     ; end

  ValidOptions  = ['m','i','x']
  ValidKcode    = [?n,?e,?s,?u]
  KcodeValue    = [16,32,48,64]

  IGNORECASE    = 1
  EXTENDED      = 2
  MULTILINE     = 4
  OPTION_MASK   = 7

  KCODE_ASCII   = 0
  KCODE_NONE    = 16
  KCODE_EUC     = 32
  KCODE_SJIS    = 48
  KCODE_UTF8    = 64
  KCODE_MASK    = 112

  # Constructs a new regular expression from the given
  # pattern. The pattern may either be a String or a
  # Regexp. If given a Regexp, options are copied from
  # the pattern and any options given are not honoured.
  # If the pattern is a String, additional options may
  # be given.
  #
  # The first optional argument can either be a Fixnum
  # representing one or more of the Regexp options ORed
  # together (Regexp::IGNORECASE, EXTENDED and MULTILINE)
  # or a flag to toggle case sensitivity. If opts is nil
  # or false, the match is case sensitive. If opts is
  # any non-nil, non-false and non-Fixnum object, its
  # presence makes the regexp case insensitive (the obj
  # is not used in any way.)
  #
  # The second optional argument can be used to enable
  # multibyte support (which is disabled by default.)
  # The flag must be one of the following strings in
  # any combination of upper- and lowercase:
  #
  # * 'e', 'euc'  for EUC
  # * 's', 'sjis' for SJIS
  # * 'u', 'utf8' for UTF-8
  #
  # You may also explicitly pass in 'n', 'N' or 'none'
  # to disable multibyte support. Any other values are
  # ignored.
  def self.new(pattern, opts = nil, lang = nil)
    if pattern.is_a?(Regexp)
      opts = pattern.options
      pattern  = pattern.source
    elsif opts.kind_of?(Fixnum)
      opts = opts & (OPTION_MASK | KCODE_MASK) if opts > 0
    elsif opts
      opts = IGNORECASE
    else
      opts = 0
    end

    if opts and lang and lang.kind_of?(String)
      opts &= OPTION_MASK
      idx   = ValidKcode.index(lang.downcase[0])
      opts |= KcodeValue[idx] if idx
    end

    if self.class.equal? Regexp
      return __regexp_new__(pattern, opts)
    else
      r = __regexp_new__(pattern, opts)
      r.send :initialize, pattern, opts, lang
      return r
    end
  end

  # FIXME - Optimize me using String#[], String#chr, etc.
  # Do away with the control-character comparisons.
  def self.escape(str)
    meta = %w![ ] { } ( ) | - * . \\ ? + ^ $ #!
    quoted = ""
    str.codepoints.each do |c|
      quoted << if meta.include?(c)
      "\\#{c}"
      elsif c == "\n"
      "\\n"
      elsif c == "\r"
      "\\r"
      elsif c == "\f"
      "\\f"
      else
        c
      end
    end
    quoted
  end

  class << self
    alias_method :compile, :new
    alias_method :quote, :escape
  end


  # See Regexp.new. This may be overridden by subclasses.
  def initialize(arg, opts, lang)
    # Nothing to do
  end

  def self.last_match(field = nil)
    match = MethodContext.current.sender.last_match
    if match
      return match if field.nil?
      return match[field]
    else
      return nil
    end
  end
  
  def self.last_match=(match)
    # Set an ivar in the sender of our sender
    parent = MethodContext.current.sender
    ctx = parent.sender
    ctx.last_match = match
  end
 
  # Different than last_match= because it sets the current last match,
  # while last_match= sets the senders last match.
  def self.my_last_match=(match)
    # Set an ivar in the sender 
    ctx = MethodContext.current.sender
    ctx.last_match = match
  end

  def self.union(*patterns)
    if patterns.nil? || patterns.length == 0
      return /(?!)/
    else
      flag  = false
      string = ""
      patterns.each do |pattern|
        string += '|' if flag
        string += pattern.to_s
        flag = true
      end
      return Regexp.new(string)
    end
  end

  def ~
    line = $_
    if !line.is_a?(String)
      Regexp.last_match = nil
      return nil
    end
    res = self.match(line)
    return res.nil? ? nil : res.begin(0)
  end

  def =~(str)
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
  
  def match_all(str)
    start = 0
    arr = []
    while(match = self.match_from(str, start))
      arr << match
      if match.collapsing?
        start += 1
      else
        start = match.end(0)
      end
    end
    arr
  end

  # def match_all_reverse(str)
  #   arr = []
  #   pos = str.size
  #   while pos >= 0
  #     match = match_region(str, pos, pos)
  #     break unless match
  #     arr << match
  #     pos = match.collapsing? ? pos - 1 : match.begin(0)
  #   end
  #   arr
  # end

  def ===(other)
    if !other.is_a?(String)
      if !other.respond_to?(:to_str)
        Regexp.last_match = nil
        return false
      end
    end
    if match = self.match_from(other.to_str, 0)
      Regexp.last_match = match
      return true
    else
      Regexp.last_match = nil
      return false
    end
  end

  def casefold?
    (options & IGNORECASE) > 0 ? true : false
  end

  def eql?(other)
    return false unless other.kind_of?(Regexp)
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
    str = '/' << source << '/' << option_to_string(options)
    k = kcode()
    str << k[0,1] if k and k != "none"
    return str
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
    Regexp.last_match = match_region(str, 0, str.size, true)
  end
  
  def match_from(str, count)
    return nil if str.nil?
    match_region(str, count, str.size, true)
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
  
  def string
    @source
  end
  
  def source
    @source
  end
  
  def full
    @full
  end
  
  def begin(idx)
   return full.at(0) if idx == 0
   return @region.at(idx - 1).at(0)
  end

  def end(idx)
   return full.at(1) if idx == 0
   @region.at(idx - 1).at(1)
  end

  def offset(idx)
   out = []
   out << self.begin(idx)
   out << self.end(idx)
   return out
  end

  def length
   @region.fields + 1
  end

  def captures
    out = []
    @region.each do |tup|
      x = tup.at(0)
      
      if x == -1
        out << nil
      else  
        y = tup.at(1)
        out << @source[x...y]
      end
    end
    return out
  end
  
  def pre_match
    return "" if full.at(0) == 0
    nd = full.at(0) - 1
    @source[0..nd]
  end
  
  def pre_match_from(idx)
    return "" if full.at(0) == 0
    nd = full.at(0) - 1
    @source[idx..nd]    
  end
  
  def collapsing?
    self.begin(0) == self.end(0)
  end
  
  def post_match
    nd = @source.size - 1
    st = full.at(1)
    @source[st..nd]
  end

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
    elsif idx < size
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
  
  def matched_area
    x = full[0]
    y = full[1]
    @source[x...y]
  end
  
  private :matched_area

  def get_capture(num)
    x, y = @region[num]
    return nil if !y or x == -1
    
    return @source[x...y]
  end

  private :get_capture

  def each_capture
    @region.each do |tup|
      x, y = *tup
      yield @source[x...y]
    end
  end

  private :each_capture

end

