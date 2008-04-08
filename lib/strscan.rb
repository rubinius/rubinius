# TODO: check these to see if they're aliases or not
#   "get_byte",           strscan_get_byte,     0);
#   "getbyte",            strscan_getbyte,      0);
#   "matched_size",       strscan_matched_size, 0);
#   "matchedsize",        strscan_matchedsize,  0);
#   "rest_size",          strscan_rest_size,    0);
#   "restsize",           strscan_restsize,     0);

# TODO: write these - they don't have tests at all (rest_size above as well)
#   "check",              strscan_check,        1);
#   "check_until",        strscan_check_until,  1);
#   "clear",              strscan_clear,        0);
#   "empty?",             strscan_empty_p,      0);
#   "exist?",             strscan_exist_p,      1);
#   "string",             strscan_get_string,   0);
#   "match?",             strscan_match_p,      1);
#   "peek",               strscan_peek,         1);
#   "peep",               strscan_peep,         1);
#   "rest",               strscan_rest,         0);
#   "rest?",              strscan_rest_p,       0);
#   "scan_full",          strscan_scan_full,    3);
#   "search_full",        strscan_search_full,  3);
#   "skip",               strscan_skip,         1);
#   "skip_until",         strscan_skip_until,   1);

class StringScanner
  Id = "bite me $Id".freeze
  Version = "1.0.0".freeze

  attr_accessor :pos
  attr_reader :match, :string

  alias :pointer :pos
  alias :pointer= :pos=

  def [] n
    match.to_a[n]
  end

  def bol?
    pos == 0 or string[pos-1..pos-1] == "\n"
  end
  alias :beginning_of_line? :bol?

  def concat str
    self.string << str
    self
  end
  alias :<< :concat # TODO: reverse

  def eos?
    _lame_guard

    self.pos >= self.string.size
  end

  def get_byte
    scan(/./mn)
  end

  def getch
    scan(/./m)
  end

  def initialize string, dup = false
    self.string = string
    self.reset
  end

  def inspect
    if defined? @string then
      rest = string.size > 5 ? string[pos..pos+4] + "..." : string
      r = if pos > 0 then
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

  def matched
    match.to_s if matched?
  end

  def matched?
    not match.nil?
  end

  def matched_size
    match.to_s.size if matched?
  end
  alias :matchedsize :matched_size

  def post_match
    match.post_match if matched?
  end

  def pre_match
    string[0...@prev_pos] if matched?
  end

  def reset
    self.pos = 0
    @match = nil
    @prev_pos = nil
    self
  end

  def scan pattern
    _lame_guard

    @prev_pos = pos
    @match = pattern.match_start(string, pos)
    if match then
      s = match.to_s
      self.pos += s.size
      s
    end
  end

  def scan_until pattern
    return scan Regexp.new("((?m-ix:.*?))" + pattern.to_s)

    # TODO: make this work
    _lame_guard

    @prev_pos = pos
    @match = pattern.search_region(string, pos, string.size, true)
    if match then
      s = match.to_s
      self.pos += s.size
      s
    end
  end

  def self.must_C_version
    # do nothing
  end

  def skip pattern
    s = scan pattern
    s.size if s
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
    self.pos = @prev_pos
    @prev_pos = nil
    @match = nil
    self
  end

  def _lame_guard
    raise ArgumentError unless defined? @string
  end
  private :_lame_guard
end
