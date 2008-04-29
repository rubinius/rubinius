# line 1 "http11_parser.rb.rl"
module Mongrel
  class HttpParserError < IOError; end
  
# line 63 "http11_parser.rb.rl"


  class HttpParser  
    SERVER_SOFTWARE   = 'Mongrel (Rubinius)'
    SERVER_PROTOCOL   = 'HTTP/1.1'
    GATEWAY_INTERFACE = 'CGI/1.2'
    HTTP_PREFIX       = 'HTTP_'
    
    
# line 16 "http11.rb"

HTTP_PARSER_ACTIONS = [
	0, 1, 0, 1, 2, 1, 3, 1, 
	4, 1, 5, 1, 6, 1, 7, 1, 
	8, 1, 9, 1, 11, 1, 12, 1, 
	13, 2, 0, 8, 2, 1, 2, 2, 
	4, 5, 2, 10, 7, 2, 12, 7, 
	3, 9, 10, 7
]


HTTP_PARSER_START = 1;

HTTP_PARSER_FIRST_FINAL = 57;

HTTP_PARSER_ERROR = 0;


HTTP_PARSER_EN_MAIN = 1;

# line 72 "http11_parser.rb.rl"
    
    def initialize
      reset
    end

    def execute(params, data, nparsed)
      len = data.size
    
      # FIXME hack replacement for write init
      p = nparsed
      pe = len
      cs = @cs
    
      raise ArgumentError, "offset past end of buffer" if nparsed > len

      
# line 65 "http11.rb"
Rubinius.asm { @labels = Hash.new { |h,k| h[k] = new_label } }
	begin
 _acts, _nacts = nil

	if ( p == pe )
		Rubinius.asm { goto @labels[:_http_parser__out] }

	end
	if ( cs == 0 )
		Rubinius.asm { goto @labels[:_http_parser__out] }

	end
Rubinius.asm { @labels[:_http_parser__resume].set! }

	case ( cs )
when 1 then
	case  data[p]
		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr0] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr0] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr0] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr0] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr0] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 0 then
	Rubinius.asm { goto @labels[:_http_parser__out] }

when 2 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr3] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr3] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr3] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr3] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr3] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 3 then
	case  data[p]
		when 42 then
Rubinius.asm { goto @labels[:_http_parser_tr4] }

		when 43 then
Rubinius.asm { goto @labels[:_http_parser_tr5] }

		when 47 then
Rubinius.asm { goto @labels[:_http_parser_tr6] }

		when 58 then
Rubinius.asm { goto @labels[:_http_parser_tr7] }

	end
	if data[p] < 65 
		if 45 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr5] }

		end
	elsif data[p] > 90 
		if 97 <= data[p] && data[p] <= 122 
			Rubinius.asm { goto @labels[:_http_parser_tr5] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr5] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 4 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr8] }

		when 35 then
Rubinius.asm { goto @labels[:_http_parser_tr9] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 5 then
	if data[p] == 72 
		Rubinius.asm { goto @labels[:_http_parser_tr10] }

end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 6 then
	if data[p] == 84 
		Rubinius.asm { goto @labels[:_http_parser_tr11] }

end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 7 then
	if data[p] == 84 
		Rubinius.asm { goto @labels[:_http_parser_tr12] }

end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 8 then
	if data[p] == 80 
		Rubinius.asm { goto @labels[:_http_parser_tr13] }

end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 9 then
	if data[p] == 47 
		Rubinius.asm { goto @labels[:_http_parser_tr14] }

end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 10 then
	if 48 <= data[p] && data[p] <= 57 
		Rubinius.asm { goto @labels[:_http_parser_tr15] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 11 then
	if data[p] == 46 
		Rubinius.asm { goto @labels[:_http_parser_tr16] }

end
	if 48 <= data[p] && data[p] <= 57 
		Rubinius.asm { goto @labels[:_http_parser_tr15] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 12 then
	if 48 <= data[p] && data[p] <= 57 
		Rubinius.asm { goto @labels[:_http_parser_tr17] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 13 then
	if data[p] == 13 
		Rubinius.asm { goto @labels[:_http_parser_tr18] }

end
	if 48 <= data[p] && data[p] <= 57 
		Rubinius.asm { goto @labels[:_http_parser_tr17] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 14 then
	if data[p] == 10 
		Rubinius.asm { goto @labels[:_http_parser_tr19] }

end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 15 then
	case  data[p]
		when 13 then
Rubinius.asm { goto @labels[:_http_parser_tr20] }

		when 33 then
Rubinius.asm { goto @labels[:_http_parser_tr21] }

		when 124 then
Rubinius.asm { goto @labels[:_http_parser_tr21] }

		when 126 then
Rubinius.asm { goto @labels[:_http_parser_tr21] }

	end
	if data[p] < 45 
		if data[p] > 39 
			if 42 <= data[p] && data[p] <= 43 
				Rubinius.asm { goto @labels[:_http_parser_tr21] }

			end
		elsif data[p] >= 35 then
			Rubinius.asm { goto @labels[:_http_parser_tr21] }

		end
	elsif data[p] > 46 
		if data[p] < 65 
			if 48 <= data[p] && data[p] <= 57 
				Rubinius.asm { goto @labels[:_http_parser_tr21] }

			end
		elsif data[p] > 90 
			if 94 <= data[p] && data[p] <= 122 
				Rubinius.asm { goto @labels[:_http_parser_tr21] }

			end
		else
			Rubinius.asm { goto @labels[:_http_parser_tr21] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr21] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 16 then
	if data[p] == 10 
		Rubinius.asm { goto @labels[:_http_parser_tr22] }

end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 57 then
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 17 then
	case  data[p]
		when 33 then
Rubinius.asm { goto @labels[:_http_parser_tr23] }

		when 58 then
Rubinius.asm { goto @labels[:_http_parser_tr24] }

		when 124 then
Rubinius.asm { goto @labels[:_http_parser_tr23] }

		when 126 then
Rubinius.asm { goto @labels[:_http_parser_tr23] }

	end
	if data[p] < 45 
		if data[p] > 39 
			if 42 <= data[p] && data[p] <= 43 
				Rubinius.asm { goto @labels[:_http_parser_tr23] }

			end
		elsif data[p] >= 35 then
			Rubinius.asm { goto @labels[:_http_parser_tr23] }

		end
	elsif data[p] > 46 
		if data[p] < 65 
			if 48 <= data[p] && data[p] <= 57 
				Rubinius.asm { goto @labels[:_http_parser_tr23] }

			end
		elsif data[p] > 90 
			if 94 <= data[p] && data[p] <= 122 
				Rubinius.asm { goto @labels[:_http_parser_tr23] }

			end
		else
			Rubinius.asm { goto @labels[:_http_parser_tr23] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr23] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 18 then
	case  data[p]
		when 13 then
Rubinius.asm { goto @labels[:_http_parser_tr26] }

		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr27] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr25] }

when 19 then
	if data[p] == 13 
		Rubinius.asm { goto @labels[:_http_parser_tr29] }

end
	Rubinius.asm { goto @labels[:_http_parser_tr28] }

when 20 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr31] }

		when 35 then
Rubinius.asm { goto @labels[:_http_parser_tr1] }

		when 37 then
Rubinius.asm { goto @labels[:_http_parser_tr32] }

		when 127 then
Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr30] }

when 21 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr34] }

		when 35 then
Rubinius.asm { goto @labels[:_http_parser_tr1] }

		when 37 then
Rubinius.asm { goto @labels[:_http_parser_tr35] }

		when 127 then
Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr33] }

when 22 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr36] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr36] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr36] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 23 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr33] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr33] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr33] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 24 then
	case  data[p]
		when 43 then
Rubinius.asm { goto @labels[:_http_parser_tr37] }

		when 58 then
Rubinius.asm { goto @labels[:_http_parser_tr38] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr37] }

		end
	elsif data[p] > 57 
		if data[p] > 90 
			if 97 <= data[p] && data[p] <= 122 
				Rubinius.asm { goto @labels[:_http_parser_tr37] }

			end
		elsif data[p] >= 65 then
			Rubinius.asm { goto @labels[:_http_parser_tr37] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr37] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 25 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr8] }

		when 35 then
Rubinius.asm { goto @labels[:_http_parser_tr9] }

		when 37 then
Rubinius.asm { goto @labels[:_http_parser_tr39] }

		when 127 then
Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr38] }

when 26 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr40] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr40] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr40] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 27 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr38] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr38] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr38] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 28 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr42] }

		when 35 then
Rubinius.asm { goto @labels[:_http_parser_tr43] }

		when 37 then
Rubinius.asm { goto @labels[:_http_parser_tr44] }

		when 59 then
Rubinius.asm { goto @labels[:_http_parser_tr45] }

		when 63 then
Rubinius.asm { goto @labels[:_http_parser_tr46] }

		when 127 then
Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr41] }

when 29 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr47] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr47] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr47] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 30 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr41] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr41] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr41] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 31 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr8] }

		when 35 then
Rubinius.asm { goto @labels[:_http_parser_tr9] }

		when 37 then
Rubinius.asm { goto @labels[:_http_parser_tr49] }

		when 63 then
Rubinius.asm { goto @labels[:_http_parser_tr50] }

		when 127 then
Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr48] }

when 32 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr51] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr51] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr51] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 33 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr48] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr48] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr48] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 34 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr53] }

		when 35 then
Rubinius.asm { goto @labels[:_http_parser_tr54] }

		when 37 then
Rubinius.asm { goto @labels[:_http_parser_tr55] }

		when 127 then
Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr52] }

when 35 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr57] }

		when 35 then
Rubinius.asm { goto @labels[:_http_parser_tr58] }

		when 37 then
Rubinius.asm { goto @labels[:_http_parser_tr59] }

		when 127 then
Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr56] }

when 36 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr60] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr60] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr60] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 37 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { goto @labels[:_http_parser_tr56] }

		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { goto @labels[:_http_parser_tr56] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr56] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 38 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr61] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr61] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr61] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr61] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr61] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 39 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr62] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr62] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr62] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr62] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr62] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 40 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr63] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr63] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr63] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr63] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr63] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 41 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr64] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr64] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr64] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr64] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr64] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 42 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr65] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr65] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr65] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr65] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr65] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 43 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr66] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr66] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr66] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr66] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr66] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 44 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr67] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr67] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr67] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr67] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr67] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 45 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr68] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr68] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr68] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr68] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr68] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 46 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr69] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr69] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr69] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr69] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr69] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 47 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr70] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr70] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr70] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr70] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr70] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 48 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr71] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr71] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr71] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr71] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr71] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 49 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr72] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr72] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr72] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr72] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr72] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 50 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr73] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr73] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr73] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr73] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr73] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 51 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr74] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr74] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr74] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr74] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr74] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 52 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr75] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr75] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr75] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr75] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr75] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 53 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr76] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr76] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr76] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr76] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr76] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 54 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr77] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr77] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr77] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr77] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr77] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 55 then
	case  data[p]
		when 32 then
Rubinius.asm { goto @labels[:_http_parser_tr2] }

		when 36 then
Rubinius.asm { goto @labels[:_http_parser_tr78] }

		when 95 then
Rubinius.asm { goto @labels[:_http_parser_tr78] }

	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { goto @labels[:_http_parser_tr78] }

		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { goto @labels[:_http_parser_tr78] }

		end
	else
		Rubinius.asm { goto @labels[:_http_parser_tr78] }

	end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

when 56 then
	if data[p] == 32 
		Rubinius.asm { goto @labels[:_http_parser_tr2] }

end
	Rubinius.asm { goto @labels[:_http_parser_tr1] }

	end # case

	Rubinius.asm { @labels[:_http_parser_tr1].set! }

cs = 0
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr0].set! }

cs = 2
Rubinius.asm { goto @labels[:_http_parser_f0] }

	Rubinius.asm { @labels[:_http_parser_tr2].set! }

cs = 3
Rubinius.asm { goto @labels[:_http_parser_f1] }

	Rubinius.asm { @labels[:_http_parser_tr4].set! }

cs = 4
Rubinius.asm { goto @labels[:_http_parser_f0] }

	Rubinius.asm { @labels[:_http_parser_tr8].set! }

cs = 5
Rubinius.asm { goto @labels[:_http_parser_f2] }

	Rubinius.asm { @labels[:_http_parser_tr31].set! }

cs = 5
Rubinius.asm { goto @labels[:_http_parser_f11] }

	Rubinius.asm { @labels[:_http_parser_tr34].set! }

cs = 5
Rubinius.asm { goto @labels[:_http_parser_f12] }

	Rubinius.asm { @labels[:_http_parser_tr42].set! }

cs = 5
Rubinius.asm { goto @labels[:_http_parser_f13] }

	Rubinius.asm { @labels[:_http_parser_tr53].set! }

cs = 5
Rubinius.asm { goto @labels[:_http_parser_f16] }

	Rubinius.asm { @labels[:_http_parser_tr57].set! }

cs = 5
Rubinius.asm { goto @labels[:_http_parser_f17] }

	Rubinius.asm { @labels[:_http_parser_tr10].set! }

cs = 6
Rubinius.asm { goto @labels[:_http_parser_f0] }

	Rubinius.asm { @labels[:_http_parser_tr11].set! }

cs = 7
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr12].set! }

cs = 8
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr13].set! }

cs = 9
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr14].set! }

cs = 10
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr15].set! }

cs = 11
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr16].set! }

cs = 12
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr17].set! }

cs = 13
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr18].set! }

cs = 14
Rubinius.asm { goto @labels[:_http_parser_f3] }

	Rubinius.asm { @labels[:_http_parser_tr26].set! }

cs = 14
Rubinius.asm { goto @labels[:_http_parser_f9] }

	Rubinius.asm { @labels[:_http_parser_tr29].set! }

cs = 14
Rubinius.asm { goto @labels[:_http_parser_f10] }

	Rubinius.asm { @labels[:_http_parser_tr19].set! }

cs = 15
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr20].set! }

cs = 16
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr21].set! }

cs = 17
Rubinius.asm { goto @labels[:_http_parser_f4] }

	Rubinius.asm { @labels[:_http_parser_tr23].set! }

cs = 17
Rubinius.asm { goto @labels[:_http_parser_f6] }

	Rubinius.asm { @labels[:_http_parser_tr24].set! }

cs = 18
Rubinius.asm { goto @labels[:_http_parser_f7] }

	Rubinius.asm { @labels[:_http_parser_tr27].set! }

cs = 18
Rubinius.asm { goto @labels[:_http_parser_f8] }

	Rubinius.asm { @labels[:_http_parser_tr28].set! }

cs = 19
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr25].set! }

cs = 19
Rubinius.asm { goto @labels[:_http_parser_f8] }

	Rubinius.asm { @labels[:_http_parser_tr9].set! }

cs = 20
Rubinius.asm { goto @labels[:_http_parser_f2] }

	Rubinius.asm { @labels[:_http_parser_tr43].set! }

cs = 20
Rubinius.asm { goto @labels[:_http_parser_f13] }

	Rubinius.asm { @labels[:_http_parser_tr54].set! }

cs = 20
Rubinius.asm { goto @labels[:_http_parser_f16] }

	Rubinius.asm { @labels[:_http_parser_tr58].set! }

cs = 20
Rubinius.asm { goto @labels[:_http_parser_f17] }

	Rubinius.asm { @labels[:_http_parser_tr33].set! }

cs = 21
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr30].set! }

cs = 21
Rubinius.asm { goto @labels[:_http_parser_f0] }

	Rubinius.asm { @labels[:_http_parser_tr35].set! }

cs = 22
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr32].set! }

cs = 22
Rubinius.asm { goto @labels[:_http_parser_f0] }

	Rubinius.asm { @labels[:_http_parser_tr36].set! }

cs = 23
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr37].set! }

cs = 24
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr5].set! }

cs = 24
Rubinius.asm { goto @labels[:_http_parser_f0] }

	Rubinius.asm { @labels[:_http_parser_tr38].set! }

cs = 25
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr7].set! }

cs = 25
Rubinius.asm { goto @labels[:_http_parser_f0] }

	Rubinius.asm { @labels[:_http_parser_tr39].set! }

cs = 26
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr40].set! }

cs = 27
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr41].set! }

cs = 28
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr6].set! }

cs = 28
Rubinius.asm { goto @labels[:_http_parser_f0] }

	Rubinius.asm { @labels[:_http_parser_tr44].set! }

cs = 29
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr47].set! }

cs = 30
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr48].set! }

cs = 31
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr45].set! }

cs = 31
Rubinius.asm { goto @labels[:_http_parser_f14] }

	Rubinius.asm { @labels[:_http_parser_tr49].set! }

cs = 32
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr51].set! }

cs = 33
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr50].set! }

cs = 34
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr46].set! }

cs = 34
Rubinius.asm { goto @labels[:_http_parser_f14] }

	Rubinius.asm { @labels[:_http_parser_tr56].set! }

cs = 35
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr52].set! }

cs = 35
Rubinius.asm { goto @labels[:_http_parser_f15] }

	Rubinius.asm { @labels[:_http_parser_tr59].set! }

cs = 36
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr55].set! }

cs = 36
Rubinius.asm { goto @labels[:_http_parser_f15] }

	Rubinius.asm { @labels[:_http_parser_tr60].set! }

cs = 37
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr3].set! }

cs = 38
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr61].set! }

cs = 39
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr62].set! }

cs = 40
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr63].set! }

cs = 41
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr64].set! }

cs = 42
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr65].set! }

cs = 43
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr66].set! }

cs = 44
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr67].set! }

cs = 45
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr68].set! }

cs = 46
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr69].set! }

cs = 47
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr70].set! }

cs = 48
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr71].set! }

cs = 49
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr72].set! }

cs = 50
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr73].set! }

cs = 51
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr74].set! }

cs = 52
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr75].set! }

cs = 53
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr76].set! }

cs = 54
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr77].set! }

cs = 55
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr78].set! }

cs = 56
Rubinius.asm { goto @labels[:_http_parser__again] }

	Rubinius.asm { @labels[:_http_parser_tr22].set! }

cs = 57
Rubinius.asm { goto @labels[:_http_parser_f5] }


Rubinius.asm { @labels[:_http_parser_f0].set! }

_acts = 1
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f6].set! }

_acts = 3
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f7].set! }

_acts = 5
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f8].set! }

_acts = 7
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f10].set! }

_acts = 9
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f1].set! }

_acts = 11
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f2].set! }

_acts = 13
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f12].set! }

_acts = 15
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f15].set! }

_acts = 17
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f3].set! }

_acts = 19
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f14].set! }

_acts = 21
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f5].set! }

_acts = 23
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f11].set! }

_acts = 25
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f4].set! }

_acts = 28
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f9].set! }

_acts = 31
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f17].set! }

_acts = 34
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f13].set! }

_acts = 37
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_f16].set! }

_acts = 40
Rubinius.asm { goto @labels[:_http_parser_execFuncs] }

Rubinius.asm { @labels[:_http_parser_execFuncs].set! }

	_nacts = HTTP_PARSER_ACTIONS[_acts]
	_acts += 1
	while ( _nacts > 0 ) 
		_nacts -= 1
		_acts += 1
		case ( HTTP_PARSER_ACTIONS[_acts-1] ) 
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
 p 		end
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
      
      params[HTTP_PREFIX + field] = value
    end
  		end
# line 12 "http11_parser.rb.rl"
	when 6 then
# line 20 "http11_parser.rb.rl"
		begin

    params['REQUEST_METHOD'] = data.slice @mark, p - @mark
  		end
# line 20 "http11_parser.rb.rl"
	when 7 then
# line 23 "http11_parser.rb.rl"
		begin

    params['REQUEST_URI'] = data.slice @mark, p - @mark
  		end
# line 23 "http11_parser.rb.rl"
	when 8 then
# line 26 "http11_parser.rb.rl"
		begin

    params['FRAGMENT'] = data.slice @mark, p - @mark
  		end
# line 26 "http11_parser.rb.rl"
	when 9 then
# line 30 "http11_parser.rb.rl"
		begin
 @query_start = p 		end
# line 30 "http11_parser.rb.rl"
	when 10 then
# line 32 "http11_parser.rb.rl"
		begin

    params['QUERY_STRING'] = data.slice @query_start, p - @query_start
  		end
# line 32 "http11_parser.rb.rl"
	when 11 then
# line 36 "http11_parser.rb.rl"
		begin

    params['SERVER_PROTOCOL'] = data.slice @mark, p - @mark
  		end
# line 36 "http11_parser.rb.rl"
	when 12 then
# line 40 "http11_parser.rb.rl"
		begin

    params['REQUEST_PATH'] = data.slice @mark, p - @mark
  		end
# line 40 "http11_parser.rb.rl"
	when 13 then
# line 44 "http11_parser.rb.rl"
		begin

    params['SERVER_SOFTWARE']   = SERVER_SOFTWARE
    params['SERVER_PROTOCOL']   = SERVER_PROTOCOL
    params['GATEWAY_INTERFACE'] = GATEWAY_INTERFACE
    
    params['CONTENT_LENGTH'] = params['HTTP_CONTENT_LENGTH'] if params['HTTP_CONTENT_LENGTH']
    params['CONTENT_TYPE']   = params['HTTP_CONTENT_TYPE']   if params['HTTP_CONTENT_TYPE']

    if params['HTTP_HOST']
      params['SERVER_NAME'], params['SERVER_PORT'] = params['HTTP_HOST'].split ':'
      params['SERVER_PORT'] ||= '80'
    end
    
    @body_start = p + 1    
    params.http_body = data.slice(p + 1, pe - p - 1) if params.respond_to? :http_body=
    Rubinius.asm { goto @labels[:_http_parser__out] }
	begin
		p += 1
		0x108804
	end

  		end
# line 44 "http11_parser.rb.rl"
# line 1850 "http11.rb"
		end
	end 
Rubinius.asm { goto @labels[:_http_parser__again] }

Rubinius.asm { @labels[:_http_parser__again].set! }

	if ( cs == 0 )
		Rubinius.asm { goto @labels[:_http_parser__out] }

	end
	p += 1
	if ( p != pe )
		Rubinius.asm { goto @labels[:_http_parser__resume] }

	end
Rubinius.asm { @labels[:_http_parser__out].set! }

	end
# line 88 "http11_parser.rb.rl"

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
        @cs = HTTP_PARSER_FIRST_FINAL
        @nread += 1
      end
    
      @nread
    end

    def reset
      @cs = HTTP_PARSER_START
    
      @body_start = 0
      @content_len = 0
      @mark = 0
      @nread = 0
      @field_len = 0
      @field_start = 0
      nil
    end
    
    def finish
      # FIXME hack to replace write eof
      @cs = HTTP_PARSER_FIRST_FINAL
    
      return -1 if error?
      return 1 if finished?
      0
    end
  
    def error?
      @cs == HTTP_PARSER_ERROR
    end
  
    def finished?
      @cs == HTTP_PARSER_FIRST_FINAL
    end
  end
end
