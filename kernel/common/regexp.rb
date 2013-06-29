# -*- encoding: us-ascii -*-

class Regexp
  IGNORECASE         = 1
  EXTENDED           = 2
  MULTILINE          = 4
  DONT_CAPTURE_GROUP = 128
  CAPTURE_GROUP      = 256

  KCODE_NONE = (1 << 9)
  KCODE_EUC  = (2 << 9)
  KCODE_SJIS = (3 << 9)
  KCODE_UTF8 = (4 << 9)
  KCODE_MASK = KCODE_NONE | KCODE_EUC | KCODE_SJIS | KCODE_UTF8

  ESCAPE_TABLE = Rubinius::Tuple.new(256)

  # Seed it with direct replacements
  i = 0
  while i < 256
    ESCAPE_TABLE[i] = i.chr
    i += 1
  end

  # The character literals (?x) are Fixnums in 1.8 and Strings in 1.9
  # so we use literal values instead so this is 1.8/1.9 compatible.
  ESCAPE_TABLE[9]   = '\\t'
  ESCAPE_TABLE[10]  = '\\n'
  ESCAPE_TABLE[11]  = '\\v'
  ESCAPE_TABLE[12]  = '\\f'
  ESCAPE_TABLE[13]  = '\\r'
  ESCAPE_TABLE[32]  = '\\ '
  ESCAPE_TABLE[35]  = '\\#'
  ESCAPE_TABLE[36]  = '\\$'
  ESCAPE_TABLE[40]  = '\\('
  ESCAPE_TABLE[41]  = '\\)'
  ESCAPE_TABLE[42]  = '\\*'
  ESCAPE_TABLE[43]  = '\\+'
  ESCAPE_TABLE[45]  = '\\-'
  ESCAPE_TABLE[46]  = '\\.'
  ESCAPE_TABLE[63]  = '\\?'
  ESCAPE_TABLE[91]  = '\\['
  ESCAPE_TABLE[92]  = '\\\\'
  ESCAPE_TABLE[93]  = '\\]'
  ESCAPE_TABLE[94]  = '\\^'
  ESCAPE_TABLE[123] = '\\{'
  ESCAPE_TABLE[124] = '\\|'
  ESCAPE_TABLE[125] = '\\}'

  class << self
    alias_method :compile, :new
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

  def match_all(str)
    start = 0
    arr = []
    while match = self.match_from(str, start)
      arr << match
      if match.collapsing?
        start += 1
      else
        start = match.full.at(1)
      end
    end
    arr
  end

  def casefold?
    (options & IGNORECASE) > 0 ? true : false
  end

  def match_from(str, count)
    return nil unless str
    search_region(str, count, str.bytesize, true)
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

  def string
    @source.dup.freeze
  end

  def source
    @source
  end

  def full
    @full
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
        val = @source.byteslice(x, y-x)
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
    return @source.byteslice(0, 0) if @full.at(0) == 0
    nd = @full.at(0) - 1
    @source.byteslice(0, nd+1)
  end

  def pre_match_from(idx)
    return @source.byteslice(0, 0) if @full.at(0) == 0
    nd = @full.at(0) - 1
    @source.byteslice(idx, nd-idx+1)
  end

  def collapsing?
    @full[0] == @full[1]
  end

  def post_match
    nd = @source.bytesize - 1
    st = @full.at(1)
    @source.byteslice(st, nd-st+1)
  end

  def inspect
    capts = captures
    if capts.empty?
      "#<MatchData \"#{matched_area}\">"
    else
      idx = 0
      capts.map! { |capture| "#{idx += 1}:#{capture.inspect}" }
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
    indexes.map { |i| self[i] }.flatten(1)
  end

  def matched_area
    x = @full.at(0)
    y = @full.at(1)
    @source.byteslice(x, y-x)
  end

  alias_method :to_s, :matched_area
  private :matched_area

  def get_capture(num)
    x, y = @region[num]
    return nil if !y or x == -1

    return @source.byteslice(x, y-x)
  end

  private :get_capture

  def each_capture
    @region.each do |tup|
      x, y = *tup
      yield @source.byteslice(x, y-x)
    end
  end

  private :each_capture

end

