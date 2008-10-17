class ScanError < StandardError; end

class StringScanner
  Id = "bite me $Id".freeze
  Version = "1.0.0".freeze

  attr_reader :pos, :match, :string

  def pos= n
    raise RangeError, "xxx" if string && n > string.size
    @pos = n
  end

  alias :pointer :pos
  alias :pointer= :pos=

  def [] n
    raise TypeError, "Bad argument #{n.inspect}" unless n.respond_to? :to_int
    match[n]
  end

  def bol?
    pos == 0 or string[pos-1..pos-1] == "\n"
  end
  alias :beginning_of_line? :bol?

  def check pattern
    _scan pattern, false, true, true
  end

  def check_until pattern
    _scan pattern, false, true, false
  end

  def clear
    warn "StringScanner#clear is obsolete; use #terminate instead" if $VERBOSE
    terminate
  end

  def concat str
    self.string << str
    self
  end
  alias :<< :concat # TODO: reverse

  def empty?
    warn "StringScanner#empty? is obsolete; use #eos? instead?" if $VERBOSE
    eos?
  end

  def eos?
    self.pos >= self.string.size
  end

  def exist? pattern
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

  def initialize string, dup = false
    self.string = string
    self.reset
  end

  def initialize_copy orig
    @match  = orig.match
    @pos    = orig.pos
    @string = orig.string
  end

  def inspect
    if defined? @string then
      rest = string.size > 5 ? string[pos..pos+4] + "..." : string
      r = if eos? then
            "#<StringScanner fin>"
          elsif pos > 0 then
            prev = string[0...pos].inspect
            "#<StringScanner #{pos}/#{string.size} #{prev} @ #{rest.inspect}>"
          else
            "#<StringScanner #{pos}/#{string.size} @ #{rest.inspect}>"
          end
      r.taint if self.string.tainted?
      r
    else
      "#<StringScanner (uninitialized)>"
    end
  end

  def match? pattern
    _scan pattern, false, false, true
  end

  def matched
    match.to_s if matched?
  end

  def matched?
    not match.nil?
  end

  def matched_size
    match.to_s.size if matched?
  end

  def matchedsize
    warn "StringScanner#matchedsize is obsolete; use #matched_size instead" if $VERBOSE
    matched_size
  end

  def post_match
    match.post_match if matched?
  end

  def pre_match
    string[0...(pos - match.to_s.size)] if matched?
  end

  def reset
    @prev_pos = self.pos = 0
    @match = nil
    self
  end

  def rest
    string[pos..-1]
  end

  def rest?
    return !eos?
  end

  def rest_size
    self.rest.size
  end

  def restsize
    warn "StringScanner#restsize is obsolete; use #rest_size instead" if $VERBOSE
    rest_size
  end

  def scan pattern
    _scan pattern, true, true, true
  end

  def scan_until pattern
    _scan pattern, true, true, false
  end

  def scan_full pattern, succptr, getstr
    _scan pattern, succptr, getstr, true
  end

  def search_full pattern, succptr, getstr
    _scan pattern, succptr, getstr, false
  end

  def self.must_C_version
    self
  end

  def skip pattern
    _scan pattern, true, false, true
  end

  def skip_until pattern
    _scan pattern, true, false, false
  end

  def string= s
    reset
    @string = s
  end

  def terminate
    @match = nil
    self.pos = string.size
    self
  end

  def unscan
    raise ScanError if @match.nil?
    self.pos = @prev_pos
    @prev_pos = nil
    @match = nil
    self
  end

  def peek len
    raise ArgumentError if len < 0
    return "" if len.zero?
    return string[pos, len]
  end

  def peep len
    warn "StringScanner#peep is obsolete; use #peek instead" if $VERBOSE
    peek len
  end

  def _scan pattern, succptr, getstr, headonly
    raise TypeError, "bad pattern argument: #{pattern.inspect}" unless
      String === pattern or Regexp === pattern or pattern.respond_to? :to_str

    @match = nil

    return nil if (string.size - pos) < 0 # TODO: make more elegant

    rest = self.rest

    if headonly then
      # NOTE - match_start is an Oniguruma feature that Rubinius exposes.
      # We use it here to avoid creating a new Regexp with '^' prepended.
      @match = pattern.match_start rest, 0
    else
      @match = pattern.match rest
    end

    return nil unless @match

    m = rest[0, @match.end(0)]

    if succptr then
      @prev_pos = pos
      self.pos += m.size
    end

    if getstr then
      m
    else
      m.size
    end
  end
  private :_scan
end
