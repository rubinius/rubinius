# depends on: class.rb

class Regexp

  ivar_as_index :__ivars__ => 0, :source => 1, :data => 2, :names => 3
  attr_reader   :__ivars__, :source, :data, :names

  ValidOptions = ['m','i','x']

  ValidKcode = [?n,?e,?s,?u]
  KcodeValue = [16,32,48,64]

  IGNORECASE  = 1
  EXTENDED    = 2
  MULTILINE   = 4
  OPTION_MASK = 7

  KCODE_ASCII = 0
  KCODE_NONE  = 16
  KCODE_EUC   = 32
  KCODE_SJIS  = 48
  KCODE_UTF8  = 64
  KCODE_MASK  = 112

  class << self
    def new(arg, opts=nil, lang=nil)
      if arg.is_a?(Regexp)
        opts = arg.options
        arg  = arg.source
      elsif opts.is_a?(Fixnum)
          opts = opts & (OPTION_MASK | KCODE_MASK)
      elsif opts
        opts = IGNORECASE
      else
        opts = 0
      end

      if !lang.nil? && !opts.nil? && lang.is_a?(String)
        opts &= OPTION_MASK
        idx   = ValidKcode.index(lang[0])
        opts |= KcodeValue[idx] if idx
      end

      __regexp_new__(arg, opts)
    end

    alias_method :compile, :new

    # FIXME - Optimize me using String#[], String#chr, etc.
    # Do away with the control-character comparisons.
    def escape(str)
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
    alias_method :quote, :escape
  end

  def self.last_match(field = nil)
    match = MethodContext.current.sender.last_match
    if match
      field.nil? ? match : match[field]
    else
      nil
    end
  end

  def self.last_match=(match)
    # Set an ivar in the sender of our sender
    parent = MethodContext.current.sender
    ctx = parent.sender
    ctx.last_match = match
  end

  def self.union(*patterns)
    return /(?!)/ if patterns.nil? || patterns.length == 0
    Regexp.new(patterns.join('|'))
  end

  def ~
    line = $_
    if !line.is_a?(String)
      Regexp.last_match = nil
      return nil
    end
    res = self.match(line)
    res.nil? ? nil : res.begin(0)
  end

  def =~(str)
    match = match_from(str, 0)
    if match
      Regexp.last_match = match
      match.begin(0)
    else
      Regexp.last_match = nil
      nil
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
      if !other.respond_to(:to_str)
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
    (source == other.source) && ( self_options == other_options)
  end

  alias_method :==, :eql?

  def hash
    str = '/' << source << '/' << option_to_string(options)
    str << (options & KCODE_MASK == 0) ? 'n' : kcode[0, 1]
    str.hash
  end

  def inspect
    '/' << source << '/' << option_to_string(options) << kcode[0, 1]
  end

  def kcode
    case options & KCODE_MASK
    when KCODE_NONE; 'none'
    when KCODE_EUC;  'euc'
    when KCODE_SJIS; 'sjis'
    when KCODE_UTF8; 'utf8'
    else nil
    end
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
    0.upto(string.length).each do |idx|
      return idx if !ValidOptions.include?(string[idx, 1])
    end
    string.length
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
  attr_reader   :source, :full
  alias_method  :source, :string
  alias_method  :matched_area, :to_s

  def begin(idx)
    return full.at(0) if idx == 0
    @region.at(idx - 1).at(0)
  end

  def end(idx)
    return full.at(1) if idx == 0
    @region.at(idx - 1).at(1)
  end

  def offset(idx)
    [self.begin(idx), self.end(idx)]
  end

  def length
    @region.fields + 1
  end

  def captures
    @region.collect do |tup|
      x = tup.at(0)
      if x == -1
        nil
      else
        y = tup.at(1)
        @source[x...y]
      end
    end
  end

  def pre_match
    pre_match_from(0)
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
      return matched_area
    elsif idx < size
      return get_capture(idx - 1)
    end
  end

  def inspect
    "#<MatchData:0x#{object_id.to_s(16)} \"#{matched_area}\">"
  end

  def select
    raise LocalJumpError, "no block given" unless block_given?

    out = []
    ma = matched_area
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
    ary = captures
    ary.unshift matched_area
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
    @source[x...y]
  end

  def each_capture
    @region.each do |tup|
      x, y = *tup
      yield @source[x...y]
    end
  end
end
