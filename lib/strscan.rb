class ScanError < StandardError; end

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

  def check pattern
    _scan pattern, false, true, true
  end

  def check_until pattern
    _scan pattern, false, true, false
  end

  def concat str
    self.string << str
    self
  end
  alias :<< :concat # TODO: reverse

  def eos?
    _lame_guard

    self.pos >= self.string.size
  end

  def exist? pattern
    _scan pattern, false, false, false
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
  alias :matchedsize :matched_size

  def post_match
    match.post_match if matched?
  end

  def pre_match
    string[0, match.begin(0)] if matched?
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
    return "" if len.zero?
    return string.substring(pos, len)
  end

  def _scan pattern, succptr, getstr, headonly
    _lame_guard

    @match = nil

    return nil if (string.size - pos) < 0 # TODO: make more elegant

    @match = if headonly then
               pattern.match_start(string, pos)
             else
               pattern.search_region(string, pos, string.size, true)
             end

    return nil if match.nil?

    m = string[pos...match.end(0)]

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

  def _lame_guard
    raise ArgumentError unless instance_variable_defined?(:@string)
  end
  private :_lame_guard
end
