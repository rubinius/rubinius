# line 1 "http11_parser.rb.rl"
module Mongrel
  class HttpParserError < IOError; end
  
# line 51 "http11_parser.rb.rl"


  class HttpParser    
    def initialize
      # FIXME this write data should *really* go somewhere else as it
      # makes the whole parser instance-specific data.
      
# line 14 "http11_parser.rb.rb"
class << self
	attr_accessor :_http_parser_actions
	private :_http_parser_actions, :_http_parser_actions=
end
self._http_parser_actions = [
	0, 1, 0, 1, 2, 1, 3, 1, 
	4, 1, 5, 1, 6, 1, 7, 1, 
	8, 1, 9, 1, 11, 1, 12, 1, 
	13, 2, 0, 8, 2, 1, 2, 2, 
	4, 5, 2, 10, 7, 2, 12, 7, 
	3, 9, 10, 7
]

class << self
	attr_accessor :_http_parser_key_offsets
	private :_http_parser_key_offsets, :_http_parser_key_offsets=
end
self._http_parser_key_offsets = [
	0, 0, 8, 17, 27, 29, 30, 31, 
	32, 33, 34, 36, 39, 41, 44, 45, 
	61, 62, 78, 80, 81, 87, 93, 99, 
	105, 115, 121, 127, 133, 141, 147, 153, 
	160, 166, 172, 178, 184, 190, 196, 205, 
	214, 223, 232, 241, 250, 259, 268, 277, 
	286, 295, 304, 313, 322, 331, 340, 349, 
	358, 359
]

class << self
	attr_accessor :_http_parser_trans_keys
	private :_http_parser_trans_keys, :_http_parser_trans_keys=
end
self._http_parser_trans_keys = [
	36, 95, 45, 46, 48, 57, 65, 90, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 42, 43, 47, 58, 45, 57, 65, 
	90, 97, 122, 32, 35, 72, 84, 84, 
	80, 47, 48, 57, 46, 48, 57, 48, 
	57, 13, 48, 57, 10, 13, 33, 124, 
	126, 35, 39, 42, 43, 45, 46, 48, 
	57, 65, 90, 94, 122, 10, 33, 58, 
	124, 126, 35, 39, 42, 43, 45, 46, 
	48, 57, 65, 90, 94, 122, 13, 32, 
	13, 32, 35, 37, 127, 0, 31, 32, 
	35, 37, 127, 0, 31, 48, 57, 65, 
	70, 97, 102, 48, 57, 65, 70, 97, 
	102, 43, 58, 45, 46, 48, 57, 65, 
	90, 97, 122, 32, 35, 37, 127, 0, 
	31, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 32, 35, 37, 
	59, 63, 127, 0, 31, 48, 57, 65, 
	70, 97, 102, 48, 57, 65, 70, 97, 
	102, 32, 35, 37, 63, 127, 0, 31, 
	48, 57, 65, 70, 97, 102, 48, 57, 
	65, 70, 97, 102, 32, 35, 37, 127, 
	0, 31, 32, 35, 37, 127, 0, 31, 
	48, 57, 65, 70, 97, 102, 48, 57, 
	65, 70, 97, 102, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 32, 36, 95, 
	45, 46, 48, 57, 65, 90, 32, 36, 
	95, 45, 46, 48, 57, 65, 90, 32, 
	36, 95, 45, 46, 48, 57, 65, 90, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 32, 36, 95, 45, 46, 48, 57, 
	65, 90, 32, 36, 95, 45, 46, 48, 
	57, 65, 90, 32, 36, 95, 45, 46, 
	48, 57, 65, 90, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 32, 36, 95, 
	45, 46, 48, 57, 65, 90, 32, 36, 
	95, 45, 46, 48, 57, 65, 90, 32, 
	36, 95, 45, 46, 48, 57, 65, 90, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 32, 36, 95, 45, 46, 48, 57, 
	65, 90, 32, 36, 95, 45, 46, 48, 
	57, 65, 90, 32, 36, 95, 45, 46, 
	48, 57, 65, 90, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 32, 36, 95, 
	45, 46, 48, 57, 65, 90, 32, 0
]

class << self
	attr_accessor :_http_parser_single_lengths
	private :_http_parser_single_lengths, :_http_parser_single_lengths=
end
self._http_parser_single_lengths = [
	0, 2, 3, 4, 2, 1, 1, 1, 
	1, 1, 0, 1, 0, 1, 1, 4, 
	1, 4, 2, 1, 4, 4, 0, 0, 
	2, 4, 0, 0, 6, 0, 0, 5, 
	0, 0, 4, 4, 0, 0, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	1, 0
]

class << self
	attr_accessor :_http_parser_range_lengths
	private :_http_parser_range_lengths, :_http_parser_range_lengths=
end
self._http_parser_range_lengths = [
	0, 3, 3, 3, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 1, 0, 6, 
	0, 6, 0, 0, 1, 1, 3, 3, 
	4, 1, 3, 3, 1, 3, 3, 1, 
	3, 3, 1, 1, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	0, 0
]

class << self
	attr_accessor :_http_parser_index_offsets
	private :_http_parser_index_offsets, :_http_parser_index_offsets=
end
self._http_parser_index_offsets = [
	0, 0, 6, 13, 21, 24, 26, 28, 
	30, 32, 34, 36, 39, 41, 44, 46, 
	57, 59, 70, 73, 75, 81, 87, 91, 
	95, 102, 108, 112, 116, 124, 128, 132, 
	139, 143, 147, 153, 159, 163, 167, 174, 
	181, 188, 195, 202, 209, 216, 223, 230, 
	237, 244, 251, 258, 265, 272, 279, 286, 
	293, 295
]

class << self
	attr_accessor :_http_parser_indicies
	private :_http_parser_indicies, :_http_parser_indicies=
end
self._http_parser_indicies = [
	0, 0, 0, 0, 0, 1, 2, 3, 
	3, 3, 3, 3, 1, 4, 5, 6, 
	7, 5, 5, 5, 1, 8, 9, 1, 
	10, 1, 11, 1, 12, 1, 13, 1, 
	14, 1, 15, 1, 16, 15, 1, 17, 
	1, 18, 17, 1, 19, 1, 20, 21, 
	21, 21, 21, 21, 21, 21, 21, 21, 
	1, 22, 1, 23, 24, 23, 23, 23, 
	23, 23, 23, 23, 23, 1, 26, 27, 
	25, 29, 28, 31, 1, 32, 1, 1, 
	30, 34, 1, 35, 1, 1, 33, 36, 
	36, 36, 1, 33, 33, 33, 1, 37, 
	38, 37, 37, 37, 37, 1, 8, 9, 
	39, 1, 1, 38, 40, 40, 40, 1, 
	38, 38, 38, 1, 42, 43, 44, 45, 
	46, 1, 1, 41, 47, 47, 47, 1, 
	41, 41, 41, 1, 8, 9, 49, 50, 
	1, 1, 48, 51, 51, 51, 1, 48, 
	48, 48, 1, 53, 54, 55, 1, 1, 
	52, 57, 58, 59, 1, 1, 56, 60, 
	60, 60, 1, 56, 56, 56, 1, 2, 
	61, 61, 61, 61, 61, 1, 2, 62, 
	62, 62, 62, 62, 1, 2, 63, 63, 
	63, 63, 63, 1, 2, 64, 64, 64, 
	64, 64, 1, 2, 65, 65, 65, 65, 
	65, 1, 2, 66, 66, 66, 66, 66, 
	1, 2, 67, 67, 67, 67, 67, 1, 
	2, 68, 68, 68, 68, 68, 1, 2, 
	69, 69, 69, 69, 69, 1, 2, 70, 
	70, 70, 70, 70, 1, 2, 71, 71, 
	71, 71, 71, 1, 2, 72, 72, 72, 
	72, 72, 1, 2, 73, 73, 73, 73, 
	73, 1, 2, 74, 74, 74, 74, 74, 
	1, 2, 75, 75, 75, 75, 75, 1, 
	2, 76, 76, 76, 76, 76, 1, 2, 
	77, 77, 77, 77, 77, 1, 2, 78, 
	78, 78, 78, 78, 1, 2, 1, 1, 
	0
]

class << self
	attr_accessor :_http_parser_trans_targs_wi
	private :_http_parser_trans_targs_wi, :_http_parser_trans_targs_wi=
end
self._http_parser_trans_targs_wi = [
	2, 0, 3, 38, 4, 24, 28, 25, 
	5, 20, 6, 7, 8, 9, 10, 11, 
	12, 13, 14, 15, 16, 17, 57, 17, 
	18, 19, 14, 18, 19, 14, 21, 5, 
	22, 21, 5, 22, 23, 24, 25, 26, 
	27, 28, 5, 20, 29, 31, 34, 30, 
	31, 32, 34, 33, 35, 5, 20, 36, 
	35, 5, 20, 36, 37, 39, 40, 41, 
	42, 43, 44, 45, 46, 47, 48, 49, 
	50, 51, 52, 53, 54, 55, 56
]

class << self
	attr_accessor :_http_parser_trans_actions_wi
	private :_http_parser_trans_actions_wi, :_http_parser_trans_actions_wi=
end
self._http_parser_trans_actions_wi = [
	1, 0, 11, 0, 1, 1, 1, 1, 
	13, 13, 1, 0, 0, 0, 0, 0, 
	0, 0, 19, 0, 0, 28, 23, 3, 
	5, 7, 31, 7, 0, 9, 1, 25, 
	1, 0, 15, 0, 0, 0, 0, 0, 
	0, 0, 37, 37, 0, 21, 21, 0, 
	0, 0, 0, 0, 17, 40, 40, 17, 
	0, 34, 34, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
]

class << self
	attr_accessor :http_parser_start
end
self.http_parser_start = 1;
class << self
	attr_accessor :http_parser_first_final
end
self.http_parser_first_final = 57;
class << self
	attr_accessor :http_parser_error
end
self.http_parser_error = 0;

class << self
	attr_accessor :http_parser_en_main
end
self.http_parser_en_main = 1;

# line 58 "http11_parser.rb.rl"
      
      @cs = http_parser_start
      
      @body_start = 0
      @content_len = 0
      @mark = 0
      @nread = 0
      @field_len = 0
      @field_start = 0
    end

    def execute(params, data, nparsed)
      len = data.size
    
      # FIXME hack replacement for write init
      p = nparsed
      pe = len
      cs = @cs
    
      raise ArgumentError, "offset past end of buffer" if nparsed > len

      
# line 260 "http11_parser.rb.rb"
begin
	_klen, _trans, _keys, _acts, _nacts = nil
	_goto_level = 0
	_resume = 10
	_eof_trans = 15
	_again = 20
	_test_eof = 30
	_out = 40
	while true
	_trigger_goto = false
	if _goto_level <= 0
	if p == pe
		_goto_level = _test_eof
		next
	end
	if cs == 0
		_goto_level = _out
		next
	end
	end
	if _goto_level <= _resume
	_keys = _http_parser_key_offsets[cs]
	_trans = _http_parser_index_offsets[cs]
	_klen = _http_parser_single_lengths[cs]
	_break_match = false
	
	begin
	  if _klen > 0
	     _lower = _keys
	     _upper = _keys + _klen - 1

	     loop do
	        break if _upper < _lower
	        _mid = _lower + ( (_upper - _lower) >> 1 )

	        if data[p] < _http_parser_trans_keys[_mid]
	           _upper = _mid - 1
	        elsif data[p] > _http_parser_trans_keys[_mid]
	           _lower = _mid + 1
	        else
	           _trans += (_mid - _keys)
	           _break_match = true
	           break
	        end
	     end # loop
	     break if _break_match
	     _keys += _klen
	     _trans += _klen
	  end
	  _klen = _http_parser_range_lengths[cs]
	  if _klen > 0
	     _lower = _keys
	     _upper = _keys + (_klen << 1) - 2
	     loop do
	        break if _upper < _lower
	        _mid = _lower + (((_upper-_lower) >> 1) & ~1)
	        if data[p] < _http_parser_trans_keys[_mid]
	          _upper = _mid - 2
	        elsif data[p] > _http_parser_trans_keys[_mid+1]
	          _lower = _mid + 2
	        else
	          _trans += ((_mid - _keys) >> 1)
	          _break_match = true
	          break
	        end
	     end # loop
	     break if _break_match
	     _trans += _klen
	  end
	end while false
	_trans = _http_parser_indicies[_trans]
	cs = _http_parser_trans_targs_wi[_trans]
	if _http_parser_trans_actions_wi[_trans] != 0
		_acts = _http_parser_trans_actions_wi[_trans]
		_nacts = _http_parser_actions[_acts]
		_acts += 1
		while _nacts > 0
			_nacts -= 1
			_acts += 1
			case _http_parser_actions[_acts - 1]
when 0 then
# line 7 "http11_parser.rb.rl"
		begin
 @mark = p 		end
# line 7 "http11_parser.rb.rl"
when 1 then
# line 8 "http11_parser.rb.rl"
		begin
 @field_start = p 		end
# line 8 "http11_parser.rb.rl"
when 2 then
# line 9 "http11_parser.rb.rl"
		begin
 snake_upcase_char p 		end
# line 9 "http11_parser.rb.rl"
when 3 then
# line 10 "http11_parser.rb.rl"
		begin
 @field_len = p - @field_start 		end
# line 10 "http11_parser.rb.rl"
when 4 then
# line 11 "http11_parser.rb.rl"
		begin
 @mark = p 		end
# line 11 "http11_parser.rb.rl"
when 5 then
# line 12 "http11_parser.rb.rl"
		begin

    if params.respond_to? :[]=
      field = data.slice(@field_start, @field_len).split('-').map { |f| f.upcase }.join('_')
      value = data.slice(@mark, p - @mark)
      
      params['HTTP_' + field] = value
      params['SERVER_NAME'] = value if field == 'HOST'
    end
  		end
# line 12 "http11_parser.rb.rl"
when 6 then
# line 21 "http11_parser.rb.rl"
		begin

    params['REQUEST_METHOD'] = data.slice @mark, p - @mark
  		end
# line 21 "http11_parser.rb.rl"
when 7 then
# line 24 "http11_parser.rb.rl"
		begin

    params['REQUEST_URI'] = data.slice @mark, p - @mark
  		end
# line 24 "http11_parser.rb.rl"
when 8 then
# line 27 "http11_parser.rb.rl"
		begin

    params['FRAGMENT'] = data.slice @mark, p - @mark
  		end
# line 27 "http11_parser.rb.rl"
when 9 then
# line 31 "http11_parser.rb.rl"
		begin
 @query_start = p 		end
# line 31 "http11_parser.rb.rl"
when 10 then
# line 33 "http11_parser.rb.rl"
		begin

    params['QUERY_STRING'] = data.slice @query_start, p - @query_start
  		end
# line 33 "http11_parser.rb.rl"
when 11 then
# line 37 "http11_parser.rb.rl"
		begin

    params['SERVER_PROTOCOL'] = data.slice @mark, p - @mark
  		end
# line 37 "http11_parser.rb.rl"
when 12 then
# line 41 "http11_parser.rb.rl"
		begin

    params['REQUEST_PATH'] = data.slice @mark, p - @mark
  		end
# line 41 "http11_parser.rb.rl"
when 13 then
# line 45 "http11_parser.rb.rl"
		begin

    @body_start = p + 1    
    	begin
		p += 1
		_trigger_goto = true
		_goto_level = _out
		break
	end

  		end
# line 45 "http11_parser.rb.rl"
# line 440 "http11_parser.rb.rb"
			end # action switch
		end
	end
	if _trigger_goto
		next
	end
	end
	if _goto_level <= _again
	if cs == 0
		_goto_level = _out
		next
	end
	p += 1
	if p != pe
		_goto_level = _resume
		next
	end
	end
	if _goto_level <= _test_eof
	end
	if _goto_level <= _out
		break
	end
	end
	end
# line 80 "http11_parser.rb.rl"

      @cs = cs
      @nread += (p - nparsed)
    
      raise HttpParserError, "buffer overflow after parsing execute"      if p > pe
      raise HttpParserError, "nread longer than length"                   if @nread > len
      raise HttpParserError, "body starts after buffer end"               if @body_start > len
      raise HttpParserError, "mark is after buffer end"                   if @mark >= len
      raise HttpParserError, "field has length longer than whole buffer"  if @field_len > len
      raise HttpParserError, "field starts after buffer end"              if @field_start >= len
    
      if @body_start > 0
        #final \r\n combo encountered so stop right here
        # FIXME hack to replace write eof
        @cs = http_parser_first_final
        @nread += 1
      end
    
      @nread
    end

    def finish
      # FIXME hack to replace write eof
      @cs = http_parser_first_final
    
      return -1 if error?
      return 1 if finished?
      0
    end
  
    def error?
      @cs == http_parser_error
    end
  
    def finished?
      @cs == http_parser_first_final
    end
  
    def snake_upcase_char(c)
      case c.chr
      when /a-z/ then c.chr.upcase.ord
      when '-' then '_'.ord
      else c
      end
    end
  end
end