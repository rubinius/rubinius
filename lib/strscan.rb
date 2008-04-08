
class StringScanner
  Id = "bite me $Id".freeze
  Version = "1.0.0".freeze

  attr_accessor :pos
  attr_reader :match, :string

  def self.must_C_version
    # do nothing
  end

  def [] n
    match.to_a[n]
  end

  def bol?
    pos == 0 or string[pos-1..pos-1] == "\n"
  end

  def concat str
    self.string << str
  end

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
p :pre_match => [string, pos, @prev_pos, match.pre_match, match.to_s]
    string[0...@prev_pos] if matched?
#    match.pre_match if matched?
  end

  def reset
    self.pos = 0
    @match = nil
    @prev_pos = nil
  end

  def scan pattern
    _lame_guard

    @prev_pos = pos
    @match = pattern.match_region(string, pos, string.size, true)
    if match then
      s = match.to_s
      self.pos += s.size
      s
    end
  end

  def scan_until pattern
    scan Regexp.new("(?m-ix:.*?)" + pattern.to_s)
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
  end

  def unscan
    self.pos = @prev_pos
    @prev_pos = nil
    @match = nil
  end

#   rb_define_method(StringScanner, "reset",       strscan_reset,       0);
#   rb_define_method(StringScanner, "terminate",   strscan_terminate,   0);
#   rb_define_method(StringScanner, "clear",       strscan_clear,       0);
#   rb_define_method(StringScanner, "string",      strscan_get_string,  0);
#   rb_define_method(StringScanner, "string=",     strscan_set_string,  1);
#   rb_define_method(StringScanner, "concat",      strscan_concat,      1);
#   rb_define_method(StringScanner, "<<",          strscan_concat,      1);
#   rb_define_method(StringScanner, "pos",         strscan_get_pos,     0);
#   rb_define_method(StringScanner, "pos=",        strscan_set_pos,     1);
#   rb_define_method(StringScanner, "pointer",     strscan_get_pos,     0);
#   rb_define_method(StringScanner, "pointer=",    strscan_set_pos,     1);

#   rb_define_method(StringScanner, "scan",        strscan_scan,        1);
#   rb_define_method(StringScanner, "skip",        strscan_skip,        1);
#   rb_define_method(StringScanner, "match?",      strscan_match_p,     1);
#   rb_define_method(StringScanner, "check",       strscan_check,       1);
#   rb_define_method(StringScanner, "scan_full",   strscan_scan_full,   3);

#   rb_define_method(StringScanner, "scan_until",  strscan_scan_until,  1);
#   rb_define_method(StringScanner, "skip_until",  strscan_skip_until,  1);
#   rb_define_method(StringScanner, "exist?",      strscan_exist_p,     1);
#   rb_define_method(StringScanner, "check_until", strscan_check_until, 1);
#   rb_define_method(StringScanner, "search_full", strscan_search_full, 3);

#   rb_define_method(StringScanner, "getch",       strscan_getch,       0);
#   rb_define_method(StringScanner, "get_byte",    strscan_get_byte,    0);
#   rb_define_method(StringScanner, "getbyte",     strscan_getbyte,     0);
#   rb_define_method(StringScanner, "peek",        strscan_peek,        1);
#   rb_define_method(StringScanner, "peep",        strscan_peep,        1);

#   rb_define_method(StringScanner, "unscan",      strscan_unscan,      0);

#   rb_define_method(StringScanner, "beginning_of_line?", strscan_bol_p, 0);
#   rb_alias(StringScanner, rb_intern("bol?"), rb_intern("beginning_of_line?"));
#   rb_define_method(StringScanner, "eos?",        strscan_eos_p,       0);
#   rb_define_method(StringScanner, "empty?",      strscan_empty_p,     0);
#   rb_define_method(StringScanner, "rest?",       strscan_rest_p,      0);

#   rb_define_method(StringScanner, "matched?",    strscan_matched_p,   0);
#   rb_define_method(StringScanner, "matched",     strscan_matched,     0);
#   rb_define_method(StringScanner, "matched_size", strscan_matched_size, 0);
#   rb_define_method(StringScanner, "matchedsize", strscan_matchedsize, 0);
#   rb_define_method(StringScanner, "[]",          strscan_aref,        1);
#   rb_define_method(StringScanner, "pre_match",   strscan_pre_match,   0);
#   rb_define_method(StringScanner, "post_match",  strscan_post_match,  0);

#   rb_define_method(StringScanner, "rest",        strscan_rest,        0);
#   rb_define_method(StringScanner, "rest_size",   strscan_rest_size,   0);
#   rb_define_method(StringScanner, "restsize",    strscan_restsize,    0);

#   rb_define_method(StringScanner, "inspect",     strscan_inspect,     0);

  def _lame_guard
    raise ArgumentError unless defined? @string
  end
  private :_lame_guard
end
