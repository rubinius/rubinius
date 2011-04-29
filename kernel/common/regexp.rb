class Regexp
  ValidKcode    = [?n,?e,?s,?u]
  KcodeValue    = [16,32,48,64]

  IGNORECASE         = 1
  EXTENDED           = 2
  MULTILINE          = 4
  DONT_CAPTURE_GROUP = 128
  CAPTURE_GROUP      = 256
  OPTION_MASK        = IGNORECASE | EXTENDED | MULTILINE | DONT_CAPTURE_GROUP | CAPTURE_GROUP

  KCODE_ASCII   = 0
  KCODE_NONE    = 16
  KCODE_EUC     = 32
  KCODE_SJIS    = 48
  KCODE_UTF8    = 64
  KCODE_MASK    = 112

  ESCAPE_TABLE  = Rubinius::Tuple.new(256)

  # Seed it with direct replacements
  i = 0
  while i < 256
    ESCAPE_TABLE[i] = i.chr
    i += 1
  end

  ESCAPE_TABLE[?\ ] = '\\ ' # '?\ ' is a space
  ESCAPE_TABLE[?[ ] = '\\['
  ESCAPE_TABLE[?] ] = '\\]'
  ESCAPE_TABLE[?{ ] = '\\{'
  ESCAPE_TABLE[?} ] = '\\}'
  ESCAPE_TABLE[?( ] = '\\('
  ESCAPE_TABLE[?) ] = '\\)'
  ESCAPE_TABLE[?| ] = '\\|'
  ESCAPE_TABLE[?- ] = '\\-'
  ESCAPE_TABLE[?* ] = '\\*'
  ESCAPE_TABLE[?. ] = '\\.'
  ESCAPE_TABLE[?\\] = '\\\\'
  ESCAPE_TABLE[?? ] = '\\?'
  ESCAPE_TABLE[?+ ] = '\\+'
  ESCAPE_TABLE[?^ ] = '\\^'
  ESCAPE_TABLE[?$ ] = '\\$'
  ESCAPE_TABLE[?# ] = '\\#'
  ESCAPE_TABLE[?\n] = '\\n'
  ESCAPE_TABLE[?\r] = '\\r'
  ESCAPE_TABLE[?\f] = '\\f'
  ESCAPE_TABLE[?\t] = '\\t'
  ESCAPE_TABLE[?\v] = '\\v'

  ##
  # Constructs a new regular expression from the given pattern. The pattern
  # may either be a String or a Regexp. If given a Regexp, options are copied
  # from the pattern and any options given are not honoured. If the pattern is
  # a String, additional options may be given.
  #
  # The first optional argument can either be a Fixnum representing one or
  # more of the Regexp options ORed together (Regexp::IGNORECASE, EXTENDED and
  # MULTILINE) or a flag to toggle case sensitivity. If opts is nil or false,
  # the match is case sensitive. If opts is any non-nil, non-false and
  # non-Fixnum object, its presence makes the regexp case insensitive (the obj
  # is not used in any way.)
  #
  # The second optional argument can be used to enable multibyte support
  # (which is disabled by default.) The flag must be one of the following
  # strings in any combination of upper- and lowercase:
  #
  # * 'e', 'euc'  for EUC
  # * 's', 'sjis' for SJIS
  # * 'u', 'utf8' for UTF-8
  #
  # You may also explicitly pass in 'n', 'N' or 'none' to disable multibyte
  # support. Any other values are ignored.

  def initialize(pattern, opts=nil, lang=nil)
    if pattern.kind_of?(Regexp)
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

    compile pattern, opts
  end

  def self.escape(str)
    StringValue(str).transform(ESCAPE_TABLE, true)
  end

  class << self
    alias_method :compile, :new
    alias_method :quote, :escape
  end

  def initialize_copy(other)
    initialize other.source, other.options, other.kcode
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

    kcode = nil
    str = ""
    patterns.each_with_index do |pat, idx|
      str << "|" if idx != 0

      if pat.kind_of? Regexp
        if pat_kcode = pat.kcode
          if kcode
            if kcode != pat_kcode
              raise ArgumentError, "Conflict kcodes: #{kcode} != #{pat_kcode}"
            end
          else
            kcode = pat_kcode
          end
        end

        str << pat.to_s
      else
        str << Regexp.quote(StringValue(pat))
      end
    end

    Regexp.new(str, nil, kcode)
  end

  def source
    @source.dup
  end

  def ~
    line = $_

    unless line.kind_of?(String)
      Regexp.last_match = nil
      return nil
    end

    res = match(line)
    return res ? res.begin(0) : nil
  end

  # Returns the index of the first character in the region that
  # matched or nil if there was no match. See #match for returning
  # the MatchData instead.
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
    while match = self.match_from(str, start)
      arr << match
      if match.collapsing?
        start += 1
      else
        start = match.end(0)
      end
    end
    arr
  end

  def ===(other)

    unless other.kind_of?(String)
      other = Rubinius::Type.try_convert other, String, :to_str
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

  def casefold?
    (options & IGNORECASE) > 0 ? true : false
  end

  def eql?(other)
    return false unless other.kind_of?(Regexp)
    return false unless source == other.source

    # Ruby 1.8 doesn't destinguish between KCODE_NONE (16) & not specified (0) for eql?
    o1 = options
    if o1 & KCODE_MASK == 0
      o1 += KCODE_NONE
    end

    o2 = other.options
    if o2 & KCODE_MASK == 0
      o2 += KCODE_NONE
    end

    o1 == o2
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
    # the regexp matches any / that is after anything except for a \
    escape = source.gsub(%r!(\\.)|/!) { $1 || '\/' }

    str = "/#{escape}/#{option_to_string(options)}"
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

  # Performs normal match and returns MatchData object from $~ or nil.
  def match(str)
    unless str
      Regexp.last_match = nil
      return nil
    end

    str = StringValue(str)

    Regexp.last_match = search_region(str, 0, str.size, true)
  end

  def match_from(str, count)
    return nil unless str
    search_region(str, count, str.size, true)
  end

  class SourceParser
    class Part
      OPTIONS_MAP = {
        'm' => Regexp::MULTILINE,
        'i' => Regexp::IGNORECASE,
        'x' => Regexp::EXTENDED
      }

      attr_accessor :options
      attr_accessor :source

      def initialize(source="")
        @source = source
        @options = []
        @negated_options = []
      end

      def <<(str)
        @source << str
      end

      def empty?
        @source.empty?
      end

      def flatten
      end

      def to_s
        # Put in the proper \'s to escape /'s
        # This is the same regexp used by #inspect
        source.gsub(%r!(\\.)|/!) { $1 || '\/' }
      end

      def has_options!
        @has_options = true
      end

      def has_options?
        @has_options
      end
    end

    class OptionsGroupPart < Part
      def to_s
        @flatten ? "#{source}" : "(#{options_string}#{source})"
      end

      def push_option!(identifier)
        @options << identifier
      end

      def push_negated_option!(identifier)
        @negated_options << identifier
      end

      def flatten
        @flatten = true
      end

      def options_string
        string = @options.join + (@negated_options.empty? ? "" : @negated_options.join)
        "?#{string}:"
      end
      private :options_string
    end

    class LookAheadGroupPart < Part
      def to_s
        "(#{source})"
      end
    end

    def initialize(source, options = 0)
      @source = source
      @options = options
      @parts = [Part.new]
    end


    def string
      "(?#{options_string}:#{parts_string})"
    end

    def parts_string
      if parts.size == 1 && parts.first.has_options?
        parts.first.flatten
      end
      parts.map { |part| part.to_s }.join
    end

    def parts
      return @parts if @already_parsed
      @index = 0
      create_parts
      @parts.reject! { |part| part.empty? }
      @already_parsed = true
      @parts
    end

    # TODO: audit specs for this method when specs are running
    def create_parts
      return unless @index < @source.size
      char =  @source[@index].chr
      case char
      when '('
        idx = @index + 1
        if idx < @source.size and @source[idx].chr == '?'
          process_group
        else
          push_current_character!
        end
      else
        push_current_character!
      end
      create_parts
    end

    def process_group
      @index += 1
      @parts << group_part_class.new

      if in_group_with_options?
        @index += 1
        process_group_options
      end

      process_look_ahead if in_lookahead_group?
      process_until_group_finished
      add_part!
    end

    def group_part_class
      if in_group_with_options?
        OptionsGroupPart
      else
        LookAheadGroupPart
      end
    end

    def in_lookahead_group?
      @source[@index, 2] == "?=" || @source[@index, 2] == "?!"
    end

    def process_look_ahead
      push_current_character!
      push_current_character!
    end

    def in_group_with_options?
      return false if @source[@index, 1] != '?'

      @source[@index + 1..-1].each_byte do |b|
        c = b.chr
        return true if ':' == c
        return false unless %w[m i x -].include? c
      end
    end

    def process_group_options
      @parts.last.has_options!
      case @source[@index].chr
      when ')'
        return
      when ':'
        @index += 1
        return
      else
        push_option!
        process_group_options
      end
    end

    def process_until_group_finished
      if @source[@index].chr == ")"
        @index += 1
        return
      else
        push_current_character!
        process_until_group_finished
      end
    end

    def push_current_character!
      @parts.last << @source[@index].chr
      @index += 1
    end

    def push_option!
      @parts.last.push_option!(@source[@index].chr)
      @index += 1
    end

    def add_part!
      @parts << Part.new
    end

    PossibleOptions = [[MULTILINE, "m"], [IGNORECASE, "i"], [EXTENDED, "x"]]
    def options_string
      chosen_options = []
      possible_options = PossibleOptions
      possible_options.each do |flag, identifier|
        chosen_options << identifier if @options & flag > 0
      end

      if parts.size == 1
        chosen_options.concat parts.first.options
      end

      excluded_options = possible_options.map { |e| e.last }.select do |identifier|
        !chosen_options.include?(identifier)
      end

      options_to_return = chosen_options
      if !excluded_options.empty?
        options_to_return << "-" << excluded_options
      end
      options_to_return.join
    end

  end

  def to_s
    SourceParser.new(source, options).string
  end

  def option_to_string(option)
    string = ""
    string << 'm' if (option & MULTILINE) > 0
    string << 'i' if (option & IGNORECASE) > 0
    string << 'x' if (option & EXTENDED) > 0
    string
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
      @names.each do |k,v|
        hash[k.to_s] = [v + 1] # we only have one location currently for a key
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
      ary = Array.new(@names.size)
      @names.each do |k,v|
        ary[v] = k.to_s
      end

      return ary
    else
      []
    end
  end
end

class MatchData

  def string
    @source.dup.freeze
  end

  def source
    @source
  end

  def full
    @full
  end

  def begin(idx)
    return @full.at(0) if idx == 0
    return @region.at(idx - 1).at(0)
  end

  def end(idx)
    return @full.at(1) if idx == 0
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
    out = Array.new(@region.fields)

    idx = 0
    @region.each do |tup|
      x = tup.at(0)

      if x == -1
        val = nil
      else
        y = tup.at(1)
        val = @source.substring(x, y-x)
      end

      out[idx] = val
      idx += 1
    end

    return out
  end
  
  def names
    @regexp.names
  end

  def pre_match
    return @source.substring(0, 0) if @full.at(0) == 0
    nd = @full.at(0) - 1
    @source.substring(0, nd+1)
  end

  def pre_match_from(idx)
    return @source.substring(0, 0) if @full.at(0) == 0
    nd = @full.at(0) - 1
    @source.substring(idx, nd-idx+1)
  end

  def collapsing?
    @full[0] == @full[1]
  end

  def post_match
    nd = @source.size - 1
    st = @full.at(1)
    @source.substring(st, nd-st+1)
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
        return @source.substring(x, y-x)
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

  def inspect
    capts = captures
    if capts.empty?
      "#<MatchData \"#{matched_area}\">"
    else
      idx = 0
      capts.map! {|capture| "#{idx += 1}:#{capture.inspect}"}
      "#<MatchData \"#{matched_area}\" #{capts.join(" ")}>"
    end
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
    x = @full.at(0)
    y = @full.at(1)
    @source.substring(x, y-x)
  end

  alias_method :to_s, :matched_area
  private :matched_area

  def get_capture(num)
    x, y = @region[num]
    return nil if !y or x == -1

    return @source.substring(x, y-x)
  end

  private :get_capture

  def each_capture
    @region.each do |tup|
      x, y = *tup
      yield @source.substring(x, y-x)
    end
  end

  private :each_capture

end

