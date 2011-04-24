class ScanError < StandardError; end

class StringScanner
  Id = "None$Id".freeze
  Version = "1.0.0".freeze

  attr_reader :pos, :match, :prev_pos

  def pos=(n)
    n = Integer(n)

    n += @string.size if n < 0

    if n < 0 or n > @string.size
      raise RangeError, "index out of range (#{n})"
    end

    @pos = n
  end

  alias_method :pointer, :pos
  alias_method :pointer=, :pos=

  def [](n)
    raise TypeError, "Bad argument #{n.inspect}" unless n.respond_to? :to_int
    @match[n] if @match
  end

  def bol?
    @pos == 0 or @string[pos-1] == ?\n
  end

  alias_method :beginning_of_line?, :bol?

  def check(pattern)
    _scan pattern, false, true, true
  end

  def check_until(pattern)
    _scan pattern, false, true, false
  end

  def clear
    warn "StringScanner#clear is obsolete; use #terminate instead" if $VERBOSE
    terminate
  end

  def concat(str)
    @string << str
    self
  end
  alias_method :<<, :concat

  def empty?
    warn "StringScanner#empty? is obsolete; use #eos? instead?" if $VERBOSE
    eos?
  end

  def eos?
    @pos >= @string.size
  end

  def exist?(pattern)
    _scan pattern, false, false, false
  end

  def get_byte
    scan(/./mn)
  end

  def getbyte
    warn "StringScanner#getbyte is obsolete; use #get_byte instead" if $VERBOSE
    get_byte
  end

  def getch
    scan(/./m)
  end

  def initialize(string, dup=false)
    if string.instance_of? String
      @original = string
      @string = string
    else
      @original = StringValue(string)
      @string = String.new @original
    end

    reset_state
  end

  def inspect
    if defined? @string
      if eos?
        str = "#<StringScanner fin>"
      else
        if string.size - pos > 5
          rest = "#{string[pos..pos+4]}..."
        else
          rest = string[pos..string.size]
        end

        if pos > 0
          if pos > 5
            prev = "...#{string[pos-5...pos]}"
          else
            prev = string[0...pos]
          end

          str = "#<StringScanner #{pos}/#{string.size} #{prev.inspect} @ #{rest.inspect}>"
        else
          str = "#<StringScanner #{pos}/#{string.size} @ #{rest.inspect}>"
        end
      end

      str.taint if @string.tainted?
      return str
    else
      "#<StringScanner (uninitialized)>"
    end
  end

  def match?(pattern)
    _scan pattern, false, false, true
  end

  def matched
    @match.to_s if @match
  end

  def matched?
    !!@match
  end

  def matched_size
    @match.to_s.size if @match
  end

  def matchedsize
    warn "StringScanner#matchedsize is obsolete; use #matched_size instead" if $VERBOSE
    matched_size
  end

  def post_match
    @match.post_match if @match
  end

  def pre_match
    @string.substring(0, match.begin(0)) if @match
  end

  def reset_state
    @prev_pos = @pos = 0
    @match = nil
  end
  private :reset_state

  def reset
    reset_state
    self
  end

  def rest
    @string.substring(@pos, rest_size)
  end

  def rest?
    return !eos?
  end

  def rest_size
    @string.size - @pos
  end

  def restsize
    warn "StringScanner#restsize is obsolete; use #rest_size instead" if $VERBOSE
    rest_size
  end

  def scan(pattern)
    _scan pattern, true, true, true
  end

  def scan_until(pattern)
    _scan pattern, true, true, false
  end

  def scan_full(pattern, advance_pos, getstr)
    _scan pattern, advance_pos, getstr, true
  end

  def search_full(pattern, advance_pos, getstr)
    _scan pattern, advance_pos, getstr, false
  end

  def self.must_C_version
    self
  end

  def skip(pattern)
    _scan pattern, true, false, true
  end

  def skip_until(pattern)
    _scan pattern, true, false, false
  end

  def string
    @original
  end

  def string=(string)
    reset_state

    if string.instance_of? String
      @original = string
      @string = string
    else
      @original = StringValue(string)
      @string = String.new @original
    end
  end

  def terminate
    @match = nil
    @pos = string.size
    self
  end

  def unscan
    raise ScanError if @match.nil?
    @pos = @prev_pos
    @prev_pos = nil
    @match = nil
    self
  end

  def peek(len)
    raise ArgumentError if len < 0
    return "" if len.zero?

    begin
      return @string[pos, len]
    rescue TypeError
      raise RangeError, "offset outside of possible range"
    end
  end

  def peep(len)
    warn "StringScanner#peep is obsolete; use #peek instead" if $VERBOSE
    peek len
  end

  def _scan(pattern, advance_pos, getstr, headonly)
    unless pattern.kind_of? Regexp
      raise TypeError, "bad pattern argument: #{pattern.inspect}"
    end

    @match = nil

    if headonly
      # NOTE - match_start is an Oniguruma feature that Rubinius exposes.
      # We use it here to avoid creating a new Regexp with '^' prepended.
      @match = pattern.match_start @string, @pos
    else
      # NOTE - search_from is an Oniguruma feature that Rubinius exposes.
      # We use it so we can begin the search in the middle of the string
      @match = pattern.search_from @string, @pos
    end

    return nil unless @match

    fin = @match.end(0)

    @prev_pos = @pos

    @pos = fin if advance_pos

    width = fin - @prev_pos

    return width unless getstr

    @string.substring(@prev_pos, width)
  end
  private :_scan
end
