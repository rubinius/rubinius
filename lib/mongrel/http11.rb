# line 1 "http11_parser.rb.rl"
module Mongrel
  class HttpParserError < IOError; end
  
# line 63 "http11_parser.rb.rl"


  class HttpParser  
    SERVER_SOFTWARE   = 'Mongrel (Rubinius)'
    SERVER_PROTOCOL   = 'HTTP/1.1'
    GATEWAY_INTERFACE = 'CGI/1.2'
    HTTP_PREFIX       = 'HTTP_'
    
    
# line 16 "http11_pre.rb"

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

      
# line 65 "http11_pre.rb"
	begin
 _acts, _nacts = nil

	if ( p == pe )
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__out]
   lbl = tbl[:_http_parser__out] = new_label
 end
 goto lbl
}
	end
	if ( cs == 0 )
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__out]
   lbl = tbl[:_http_parser__out] = new_label
 end
 goto lbl
}
	end
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__resume]
   lbl = tbl[:_http_parser__resume] = new_label
 end
 lbl.set!
}
	case ( cs )
when 1 then
	case  data[p]
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr0]
   lbl = tbl[:_http_parser_tr0] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr0]
   lbl = tbl[:_http_parser_tr0] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr0]
   lbl = tbl[:_http_parser_tr0] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr0]
   lbl = tbl[:_http_parser_tr0] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr0]
   lbl = tbl[:_http_parser_tr0] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 0 then
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__out]
   lbl = tbl[:_http_parser__out] = new_label
 end
 goto lbl
}
when 2 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr3]
   lbl = tbl[:_http_parser_tr3] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr3]
   lbl = tbl[:_http_parser_tr3] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr3]
   lbl = tbl[:_http_parser_tr3] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr3]
   lbl = tbl[:_http_parser_tr3] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr3]
   lbl = tbl[:_http_parser_tr3] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 3 then
	case  data[p]
		when 42 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr4]
   lbl = tbl[:_http_parser_tr4] = new_label
 end
 goto lbl
}
		when 43 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr5]
   lbl = tbl[:_http_parser_tr5] = new_label
 end
 goto lbl
}
		when 47 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr6]
   lbl = tbl[:_http_parser_tr6] = new_label
 end
 goto lbl
}
		when 58 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr7]
   lbl = tbl[:_http_parser_tr7] = new_label
 end
 goto lbl
}
	end
	if data[p] < 65 
		if 45 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr5]
   lbl = tbl[:_http_parser_tr5] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 90 
		if 97 <= data[p] && data[p] <= 122 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr5]
   lbl = tbl[:_http_parser_tr5] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr5]
   lbl = tbl[:_http_parser_tr5] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 4 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr8]
   lbl = tbl[:_http_parser_tr8] = new_label
 end
 goto lbl
}
		when 35 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr9]
   lbl = tbl[:_http_parser_tr9] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 5 then
	if data[p] == 72 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr10]
   lbl = tbl[:_http_parser_tr10] = new_label
 end
 goto lbl
}
end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 6 then
	if data[p] == 84 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr11]
   lbl = tbl[:_http_parser_tr11] = new_label
 end
 goto lbl
}
end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 7 then
	if data[p] == 84 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr12]
   lbl = tbl[:_http_parser_tr12] = new_label
 end
 goto lbl
}
end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 8 then
	if data[p] == 80 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr13]
   lbl = tbl[:_http_parser_tr13] = new_label
 end
 goto lbl
}
end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 9 then
	if data[p] == 47 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr14]
   lbl = tbl[:_http_parser_tr14] = new_label
 end
 goto lbl
}
end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 10 then
	if 48 <= data[p] && data[p] <= 57 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr15]
   lbl = tbl[:_http_parser_tr15] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 11 then
	if data[p] == 46 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr16]
   lbl = tbl[:_http_parser_tr16] = new_label
 end
 goto lbl
}
end
	if 48 <= data[p] && data[p] <= 57 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr15]
   lbl = tbl[:_http_parser_tr15] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 12 then
	if 48 <= data[p] && data[p] <= 57 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr17]
   lbl = tbl[:_http_parser_tr17] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 13 then
	if data[p] == 13 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr18]
   lbl = tbl[:_http_parser_tr18] = new_label
 end
 goto lbl
}
end
	if 48 <= data[p] && data[p] <= 57 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr17]
   lbl = tbl[:_http_parser_tr17] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 14 then
	if data[p] == 10 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr19]
   lbl = tbl[:_http_parser_tr19] = new_label
 end
 goto lbl
}
end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 15 then
	case  data[p]
		when 13 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr20]
   lbl = tbl[:_http_parser_tr20] = new_label
 end
 goto lbl
}
		when 33 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 goto lbl
}
		when 124 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 goto lbl
}
		when 126 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 goto lbl
}
	end
	if data[p] < 45 
		if data[p] > 39 
			if 42 <= data[p] && data[p] <= 43 
				Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 goto lbl
}
			end
		elsif data[p] >= 35 then
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 46 
		if data[p] < 65 
			if 48 <= data[p] && data[p] <= 57 
				Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 goto lbl
}
			end
		elsif data[p] > 90 
			if 94 <= data[p] && data[p] <= 122 
				Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 goto lbl
}
			end
		else
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 16 then
	if data[p] == 10 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr22]
   lbl = tbl[:_http_parser_tr22] = new_label
 end
 goto lbl
}
end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 57 then
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 17 then
	case  data[p]
		when 33 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 goto lbl
}
		when 58 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr24]
   lbl = tbl[:_http_parser_tr24] = new_label
 end
 goto lbl
}
		when 124 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 goto lbl
}
		when 126 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 goto lbl
}
	end
	if data[p] < 45 
		if data[p] > 39 
			if 42 <= data[p] && data[p] <= 43 
				Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 goto lbl
}
			end
		elsif data[p] >= 35 then
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 46 
		if data[p] < 65 
			if 48 <= data[p] && data[p] <= 57 
				Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 goto lbl
}
			end
		elsif data[p] > 90 
			if 94 <= data[p] && data[p] <= 122 
				Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 goto lbl
}
			end
		else
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 18 then
	case  data[p]
		when 13 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr26]
   lbl = tbl[:_http_parser_tr26] = new_label
 end
 goto lbl
}
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr27]
   lbl = tbl[:_http_parser_tr27] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr25]
   lbl = tbl[:_http_parser_tr25] = new_label
 end
 goto lbl
}
when 19 then
	if data[p] == 13 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr29]
   lbl = tbl[:_http_parser_tr29] = new_label
 end
 goto lbl
}
end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr28]
   lbl = tbl[:_http_parser_tr28] = new_label
 end
 goto lbl
}
when 20 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr31]
   lbl = tbl[:_http_parser_tr31] = new_label
 end
 goto lbl
}
		when 35 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
		when 37 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr32]
   lbl = tbl[:_http_parser_tr32] = new_label
 end
 goto lbl
}
		when 127 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr30]
   lbl = tbl[:_http_parser_tr30] = new_label
 end
 goto lbl
}
when 21 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr34]
   lbl = tbl[:_http_parser_tr34] = new_label
 end
 goto lbl
}
		when 35 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
		when 37 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr35]
   lbl = tbl[:_http_parser_tr35] = new_label
 end
 goto lbl
}
		when 127 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr33]
   lbl = tbl[:_http_parser_tr33] = new_label
 end
 goto lbl
}
when 22 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr36]
   lbl = tbl[:_http_parser_tr36] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr36]
   lbl = tbl[:_http_parser_tr36] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr36]
   lbl = tbl[:_http_parser_tr36] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 23 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr33]
   lbl = tbl[:_http_parser_tr33] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr33]
   lbl = tbl[:_http_parser_tr33] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr33]
   lbl = tbl[:_http_parser_tr33] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 24 then
	case  data[p]
		when 43 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr37]
   lbl = tbl[:_http_parser_tr37] = new_label
 end
 goto lbl
}
		when 58 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr38]
   lbl = tbl[:_http_parser_tr38] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr37]
   lbl = tbl[:_http_parser_tr37] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if data[p] > 90 
			if 97 <= data[p] && data[p] <= 122 
				Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr37]
   lbl = tbl[:_http_parser_tr37] = new_label
 end
 goto lbl
}
			end
		elsif data[p] >= 65 then
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr37]
   lbl = tbl[:_http_parser_tr37] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr37]
   lbl = tbl[:_http_parser_tr37] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 25 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr8]
   lbl = tbl[:_http_parser_tr8] = new_label
 end
 goto lbl
}
		when 35 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr9]
   lbl = tbl[:_http_parser_tr9] = new_label
 end
 goto lbl
}
		when 37 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr39]
   lbl = tbl[:_http_parser_tr39] = new_label
 end
 goto lbl
}
		when 127 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr38]
   lbl = tbl[:_http_parser_tr38] = new_label
 end
 goto lbl
}
when 26 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr40]
   lbl = tbl[:_http_parser_tr40] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr40]
   lbl = tbl[:_http_parser_tr40] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr40]
   lbl = tbl[:_http_parser_tr40] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 27 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr38]
   lbl = tbl[:_http_parser_tr38] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr38]
   lbl = tbl[:_http_parser_tr38] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr38]
   lbl = tbl[:_http_parser_tr38] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 28 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr42]
   lbl = tbl[:_http_parser_tr42] = new_label
 end
 goto lbl
}
		when 35 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr43]
   lbl = tbl[:_http_parser_tr43] = new_label
 end
 goto lbl
}
		when 37 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr44]
   lbl = tbl[:_http_parser_tr44] = new_label
 end
 goto lbl
}
		when 59 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr45]
   lbl = tbl[:_http_parser_tr45] = new_label
 end
 goto lbl
}
		when 63 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr46]
   lbl = tbl[:_http_parser_tr46] = new_label
 end
 goto lbl
}
		when 127 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr41]
   lbl = tbl[:_http_parser_tr41] = new_label
 end
 goto lbl
}
when 29 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr47]
   lbl = tbl[:_http_parser_tr47] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr47]
   lbl = tbl[:_http_parser_tr47] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr47]
   lbl = tbl[:_http_parser_tr47] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 30 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr41]
   lbl = tbl[:_http_parser_tr41] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr41]
   lbl = tbl[:_http_parser_tr41] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr41]
   lbl = tbl[:_http_parser_tr41] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 31 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr8]
   lbl = tbl[:_http_parser_tr8] = new_label
 end
 goto lbl
}
		when 35 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr9]
   lbl = tbl[:_http_parser_tr9] = new_label
 end
 goto lbl
}
		when 37 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr49]
   lbl = tbl[:_http_parser_tr49] = new_label
 end
 goto lbl
}
		when 63 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr50]
   lbl = tbl[:_http_parser_tr50] = new_label
 end
 goto lbl
}
		when 127 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr48]
   lbl = tbl[:_http_parser_tr48] = new_label
 end
 goto lbl
}
when 32 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr51]
   lbl = tbl[:_http_parser_tr51] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr51]
   lbl = tbl[:_http_parser_tr51] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr51]
   lbl = tbl[:_http_parser_tr51] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 33 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr48]
   lbl = tbl[:_http_parser_tr48] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr48]
   lbl = tbl[:_http_parser_tr48] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr48]
   lbl = tbl[:_http_parser_tr48] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 34 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr53]
   lbl = tbl[:_http_parser_tr53] = new_label
 end
 goto lbl
}
		when 35 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr54]
   lbl = tbl[:_http_parser_tr54] = new_label
 end
 goto lbl
}
		when 37 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr55]
   lbl = tbl[:_http_parser_tr55] = new_label
 end
 goto lbl
}
		when 127 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr52]
   lbl = tbl[:_http_parser_tr52] = new_label
 end
 goto lbl
}
when 35 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr57]
   lbl = tbl[:_http_parser_tr57] = new_label
 end
 goto lbl
}
		when 35 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr58]
   lbl = tbl[:_http_parser_tr58] = new_label
 end
 goto lbl
}
		when 37 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr59]
   lbl = tbl[:_http_parser_tr59] = new_label
 end
 goto lbl
}
		when 127 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	if 0 <= data[p] && data[p] <= 31 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr56]
   lbl = tbl[:_http_parser_tr56] = new_label
 end
 goto lbl
}
when 36 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr60]
   lbl = tbl[:_http_parser_tr60] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr60]
   lbl = tbl[:_http_parser_tr60] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr60]
   lbl = tbl[:_http_parser_tr60] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 37 then
	if data[p] < 65 
		if 48 <= data[p] && data[p] <= 57 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr56]
   lbl = tbl[:_http_parser_tr56] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 70 
		if 97 <= data[p] && data[p] <= 102 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr56]
   lbl = tbl[:_http_parser_tr56] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr56]
   lbl = tbl[:_http_parser_tr56] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 38 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr61]
   lbl = tbl[:_http_parser_tr61] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr61]
   lbl = tbl[:_http_parser_tr61] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr61]
   lbl = tbl[:_http_parser_tr61] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr61]
   lbl = tbl[:_http_parser_tr61] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr61]
   lbl = tbl[:_http_parser_tr61] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 39 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr62]
   lbl = tbl[:_http_parser_tr62] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr62]
   lbl = tbl[:_http_parser_tr62] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr62]
   lbl = tbl[:_http_parser_tr62] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr62]
   lbl = tbl[:_http_parser_tr62] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr62]
   lbl = tbl[:_http_parser_tr62] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 40 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr63]
   lbl = tbl[:_http_parser_tr63] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr63]
   lbl = tbl[:_http_parser_tr63] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr63]
   lbl = tbl[:_http_parser_tr63] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr63]
   lbl = tbl[:_http_parser_tr63] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr63]
   lbl = tbl[:_http_parser_tr63] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 41 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr64]
   lbl = tbl[:_http_parser_tr64] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr64]
   lbl = tbl[:_http_parser_tr64] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr64]
   lbl = tbl[:_http_parser_tr64] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr64]
   lbl = tbl[:_http_parser_tr64] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr64]
   lbl = tbl[:_http_parser_tr64] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 42 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr65]
   lbl = tbl[:_http_parser_tr65] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr65]
   lbl = tbl[:_http_parser_tr65] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr65]
   lbl = tbl[:_http_parser_tr65] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr65]
   lbl = tbl[:_http_parser_tr65] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr65]
   lbl = tbl[:_http_parser_tr65] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 43 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr66]
   lbl = tbl[:_http_parser_tr66] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr66]
   lbl = tbl[:_http_parser_tr66] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr66]
   lbl = tbl[:_http_parser_tr66] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr66]
   lbl = tbl[:_http_parser_tr66] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr66]
   lbl = tbl[:_http_parser_tr66] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 44 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr67]
   lbl = tbl[:_http_parser_tr67] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr67]
   lbl = tbl[:_http_parser_tr67] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr67]
   lbl = tbl[:_http_parser_tr67] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr67]
   lbl = tbl[:_http_parser_tr67] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr67]
   lbl = tbl[:_http_parser_tr67] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 45 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr68]
   lbl = tbl[:_http_parser_tr68] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr68]
   lbl = tbl[:_http_parser_tr68] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr68]
   lbl = tbl[:_http_parser_tr68] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr68]
   lbl = tbl[:_http_parser_tr68] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr68]
   lbl = tbl[:_http_parser_tr68] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 46 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr69]
   lbl = tbl[:_http_parser_tr69] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr69]
   lbl = tbl[:_http_parser_tr69] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr69]
   lbl = tbl[:_http_parser_tr69] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr69]
   lbl = tbl[:_http_parser_tr69] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr69]
   lbl = tbl[:_http_parser_tr69] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 47 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr70]
   lbl = tbl[:_http_parser_tr70] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr70]
   lbl = tbl[:_http_parser_tr70] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr70]
   lbl = tbl[:_http_parser_tr70] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr70]
   lbl = tbl[:_http_parser_tr70] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr70]
   lbl = tbl[:_http_parser_tr70] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 48 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr71]
   lbl = tbl[:_http_parser_tr71] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr71]
   lbl = tbl[:_http_parser_tr71] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr71]
   lbl = tbl[:_http_parser_tr71] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr71]
   lbl = tbl[:_http_parser_tr71] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr71]
   lbl = tbl[:_http_parser_tr71] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 49 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr72]
   lbl = tbl[:_http_parser_tr72] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr72]
   lbl = tbl[:_http_parser_tr72] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr72]
   lbl = tbl[:_http_parser_tr72] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr72]
   lbl = tbl[:_http_parser_tr72] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr72]
   lbl = tbl[:_http_parser_tr72] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 50 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr73]
   lbl = tbl[:_http_parser_tr73] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr73]
   lbl = tbl[:_http_parser_tr73] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr73]
   lbl = tbl[:_http_parser_tr73] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr73]
   lbl = tbl[:_http_parser_tr73] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr73]
   lbl = tbl[:_http_parser_tr73] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 51 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr74]
   lbl = tbl[:_http_parser_tr74] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr74]
   lbl = tbl[:_http_parser_tr74] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr74]
   lbl = tbl[:_http_parser_tr74] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr74]
   lbl = tbl[:_http_parser_tr74] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr74]
   lbl = tbl[:_http_parser_tr74] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 52 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr75]
   lbl = tbl[:_http_parser_tr75] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr75]
   lbl = tbl[:_http_parser_tr75] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr75]
   lbl = tbl[:_http_parser_tr75] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr75]
   lbl = tbl[:_http_parser_tr75] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr75]
   lbl = tbl[:_http_parser_tr75] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 53 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr76]
   lbl = tbl[:_http_parser_tr76] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr76]
   lbl = tbl[:_http_parser_tr76] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr76]
   lbl = tbl[:_http_parser_tr76] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr76]
   lbl = tbl[:_http_parser_tr76] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr76]
   lbl = tbl[:_http_parser_tr76] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 54 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr77]
   lbl = tbl[:_http_parser_tr77] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr77]
   lbl = tbl[:_http_parser_tr77] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr77]
   lbl = tbl[:_http_parser_tr77] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr77]
   lbl = tbl[:_http_parser_tr77] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr77]
   lbl = tbl[:_http_parser_tr77] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 55 then
	case  data[p]
		when 32 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
		when 36 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr78]
   lbl = tbl[:_http_parser_tr78] = new_label
 end
 goto lbl
}
		when 95 then
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr78]
   lbl = tbl[:_http_parser_tr78] = new_label
 end
 goto lbl
}
	end
	if data[p] < 48 
		if 45 <= data[p] && data[p] <= 46 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr78]
   lbl = tbl[:_http_parser_tr78] = new_label
 end
 goto lbl
}
		end
	elsif data[p] > 57 
		if 65 <= data[p] && data[p] <= 90 
			Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr78]
   lbl = tbl[:_http_parser_tr78] = new_label
 end
 goto lbl
}
		end
	else
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr78]
   lbl = tbl[:_http_parser_tr78] = new_label
 end
 goto lbl
}
	end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
when 56 then
	if data[p] == 32 
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 goto lbl
}
end
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 goto lbl
}
	end # case

	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr1]
   lbl = tbl[:_http_parser_tr1] = new_label
 end
 lbl.set!
}
cs = 0
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr0]
   lbl = tbl[:_http_parser_tr0] = new_label
 end
 lbl.set!
}
cs = 2
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f0]
   lbl = tbl[:_http_parser_f0] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr2]
   lbl = tbl[:_http_parser_tr2] = new_label
 end
 lbl.set!
}
cs = 3
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f1]
   lbl = tbl[:_http_parser_f1] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr4]
   lbl = tbl[:_http_parser_tr4] = new_label
 end
 lbl.set!
}
cs = 4
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f0]
   lbl = tbl[:_http_parser_f0] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr8]
   lbl = tbl[:_http_parser_tr8] = new_label
 end
 lbl.set!
}
cs = 5
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f2]
   lbl = tbl[:_http_parser_f2] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr31]
   lbl = tbl[:_http_parser_tr31] = new_label
 end
 lbl.set!
}
cs = 5
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f11]
   lbl = tbl[:_http_parser_f11] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr34]
   lbl = tbl[:_http_parser_tr34] = new_label
 end
 lbl.set!
}
cs = 5
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f12]
   lbl = tbl[:_http_parser_f12] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr42]
   lbl = tbl[:_http_parser_tr42] = new_label
 end
 lbl.set!
}
cs = 5
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f13]
   lbl = tbl[:_http_parser_f13] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr53]
   lbl = tbl[:_http_parser_tr53] = new_label
 end
 lbl.set!
}
cs = 5
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f16]
   lbl = tbl[:_http_parser_f16] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr57]
   lbl = tbl[:_http_parser_tr57] = new_label
 end
 lbl.set!
}
cs = 5
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f17]
   lbl = tbl[:_http_parser_f17] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr10]
   lbl = tbl[:_http_parser_tr10] = new_label
 end
 lbl.set!
}
cs = 6
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f0]
   lbl = tbl[:_http_parser_f0] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr11]
   lbl = tbl[:_http_parser_tr11] = new_label
 end
 lbl.set!
}
cs = 7
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr12]
   lbl = tbl[:_http_parser_tr12] = new_label
 end
 lbl.set!
}
cs = 8
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr13]
   lbl = tbl[:_http_parser_tr13] = new_label
 end
 lbl.set!
}
cs = 9
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr14]
   lbl = tbl[:_http_parser_tr14] = new_label
 end
 lbl.set!
}
cs = 10
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr15]
   lbl = tbl[:_http_parser_tr15] = new_label
 end
 lbl.set!
}
cs = 11
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr16]
   lbl = tbl[:_http_parser_tr16] = new_label
 end
 lbl.set!
}
cs = 12
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr17]
   lbl = tbl[:_http_parser_tr17] = new_label
 end
 lbl.set!
}
cs = 13
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr18]
   lbl = tbl[:_http_parser_tr18] = new_label
 end
 lbl.set!
}
cs = 14
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f3]
   lbl = tbl[:_http_parser_f3] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr26]
   lbl = tbl[:_http_parser_tr26] = new_label
 end
 lbl.set!
}
cs = 14
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f9]
   lbl = tbl[:_http_parser_f9] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr29]
   lbl = tbl[:_http_parser_tr29] = new_label
 end
 lbl.set!
}
cs = 14
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f10]
   lbl = tbl[:_http_parser_f10] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr19]
   lbl = tbl[:_http_parser_tr19] = new_label
 end
 lbl.set!
}
cs = 15
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr20]
   lbl = tbl[:_http_parser_tr20] = new_label
 end
 lbl.set!
}
cs = 16
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr21]
   lbl = tbl[:_http_parser_tr21] = new_label
 end
 lbl.set!
}
cs = 17
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f4]
   lbl = tbl[:_http_parser_f4] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr23]
   lbl = tbl[:_http_parser_tr23] = new_label
 end
 lbl.set!
}
cs = 17
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f6]
   lbl = tbl[:_http_parser_f6] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr24]
   lbl = tbl[:_http_parser_tr24] = new_label
 end
 lbl.set!
}
cs = 18
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f7]
   lbl = tbl[:_http_parser_f7] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr27]
   lbl = tbl[:_http_parser_tr27] = new_label
 end
 lbl.set!
}
cs = 18
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f8]
   lbl = tbl[:_http_parser_f8] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr28]
   lbl = tbl[:_http_parser_tr28] = new_label
 end
 lbl.set!
}
cs = 19
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr25]
   lbl = tbl[:_http_parser_tr25] = new_label
 end
 lbl.set!
}
cs = 19
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f8]
   lbl = tbl[:_http_parser_f8] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr9]
   lbl = tbl[:_http_parser_tr9] = new_label
 end
 lbl.set!
}
cs = 20
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f2]
   lbl = tbl[:_http_parser_f2] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr43]
   lbl = tbl[:_http_parser_tr43] = new_label
 end
 lbl.set!
}
cs = 20
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f13]
   lbl = tbl[:_http_parser_f13] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr54]
   lbl = tbl[:_http_parser_tr54] = new_label
 end
 lbl.set!
}
cs = 20
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f16]
   lbl = tbl[:_http_parser_f16] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr58]
   lbl = tbl[:_http_parser_tr58] = new_label
 end
 lbl.set!
}
cs = 20
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f17]
   lbl = tbl[:_http_parser_f17] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr33]
   lbl = tbl[:_http_parser_tr33] = new_label
 end
 lbl.set!
}
cs = 21
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr30]
   lbl = tbl[:_http_parser_tr30] = new_label
 end
 lbl.set!
}
cs = 21
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f0]
   lbl = tbl[:_http_parser_f0] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr35]
   lbl = tbl[:_http_parser_tr35] = new_label
 end
 lbl.set!
}
cs = 22
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr32]
   lbl = tbl[:_http_parser_tr32] = new_label
 end
 lbl.set!
}
cs = 22
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f0]
   lbl = tbl[:_http_parser_f0] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr36]
   lbl = tbl[:_http_parser_tr36] = new_label
 end
 lbl.set!
}
cs = 23
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr37]
   lbl = tbl[:_http_parser_tr37] = new_label
 end
 lbl.set!
}
cs = 24
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr5]
   lbl = tbl[:_http_parser_tr5] = new_label
 end
 lbl.set!
}
cs = 24
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f0]
   lbl = tbl[:_http_parser_f0] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr38]
   lbl = tbl[:_http_parser_tr38] = new_label
 end
 lbl.set!
}
cs = 25
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr7]
   lbl = tbl[:_http_parser_tr7] = new_label
 end
 lbl.set!
}
cs = 25
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f0]
   lbl = tbl[:_http_parser_f0] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr39]
   lbl = tbl[:_http_parser_tr39] = new_label
 end
 lbl.set!
}
cs = 26
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr40]
   lbl = tbl[:_http_parser_tr40] = new_label
 end
 lbl.set!
}
cs = 27
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr41]
   lbl = tbl[:_http_parser_tr41] = new_label
 end
 lbl.set!
}
cs = 28
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr6]
   lbl = tbl[:_http_parser_tr6] = new_label
 end
 lbl.set!
}
cs = 28
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f0]
   lbl = tbl[:_http_parser_f0] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr44]
   lbl = tbl[:_http_parser_tr44] = new_label
 end
 lbl.set!
}
cs = 29
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr47]
   lbl = tbl[:_http_parser_tr47] = new_label
 end
 lbl.set!
}
cs = 30
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr48]
   lbl = tbl[:_http_parser_tr48] = new_label
 end
 lbl.set!
}
cs = 31
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr45]
   lbl = tbl[:_http_parser_tr45] = new_label
 end
 lbl.set!
}
cs = 31
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f14]
   lbl = tbl[:_http_parser_f14] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr49]
   lbl = tbl[:_http_parser_tr49] = new_label
 end
 lbl.set!
}
cs = 32
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr51]
   lbl = tbl[:_http_parser_tr51] = new_label
 end
 lbl.set!
}
cs = 33
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr50]
   lbl = tbl[:_http_parser_tr50] = new_label
 end
 lbl.set!
}
cs = 34
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr46]
   lbl = tbl[:_http_parser_tr46] = new_label
 end
 lbl.set!
}
cs = 34
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f14]
   lbl = tbl[:_http_parser_f14] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr56]
   lbl = tbl[:_http_parser_tr56] = new_label
 end
 lbl.set!
}
cs = 35
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr52]
   lbl = tbl[:_http_parser_tr52] = new_label
 end
 lbl.set!
}
cs = 35
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f15]
   lbl = tbl[:_http_parser_f15] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr59]
   lbl = tbl[:_http_parser_tr59] = new_label
 end
 lbl.set!
}
cs = 36
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr55]
   lbl = tbl[:_http_parser_tr55] = new_label
 end
 lbl.set!
}
cs = 36
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f15]
   lbl = tbl[:_http_parser_f15] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr60]
   lbl = tbl[:_http_parser_tr60] = new_label
 end
 lbl.set!
}
cs = 37
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr3]
   lbl = tbl[:_http_parser_tr3] = new_label
 end
 lbl.set!
}
cs = 38
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr61]
   lbl = tbl[:_http_parser_tr61] = new_label
 end
 lbl.set!
}
cs = 39
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr62]
   lbl = tbl[:_http_parser_tr62] = new_label
 end
 lbl.set!
}
cs = 40
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr63]
   lbl = tbl[:_http_parser_tr63] = new_label
 end
 lbl.set!
}
cs = 41
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr64]
   lbl = tbl[:_http_parser_tr64] = new_label
 end
 lbl.set!
}
cs = 42
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr65]
   lbl = tbl[:_http_parser_tr65] = new_label
 end
 lbl.set!
}
cs = 43
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr66]
   lbl = tbl[:_http_parser_tr66] = new_label
 end
 lbl.set!
}
cs = 44
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr67]
   lbl = tbl[:_http_parser_tr67] = new_label
 end
 lbl.set!
}
cs = 45
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr68]
   lbl = tbl[:_http_parser_tr68] = new_label
 end
 lbl.set!
}
cs = 46
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr69]
   lbl = tbl[:_http_parser_tr69] = new_label
 end
 lbl.set!
}
cs = 47
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr70]
   lbl = tbl[:_http_parser_tr70] = new_label
 end
 lbl.set!
}
cs = 48
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr71]
   lbl = tbl[:_http_parser_tr71] = new_label
 end
 lbl.set!
}
cs = 49
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr72]
   lbl = tbl[:_http_parser_tr72] = new_label
 end
 lbl.set!
}
cs = 50
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr73]
   lbl = tbl[:_http_parser_tr73] = new_label
 end
 lbl.set!
}
cs = 51
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr74]
   lbl = tbl[:_http_parser_tr74] = new_label
 end
 lbl.set!
}
cs = 52
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr75]
   lbl = tbl[:_http_parser_tr75] = new_label
 end
 lbl.set!
}
cs = 53
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr76]
   lbl = tbl[:_http_parser_tr76] = new_label
 end
 lbl.set!
}
cs = 54
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr77]
   lbl = tbl[:_http_parser_tr77] = new_label
 end
 lbl.set!
}
cs = 55
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr78]
   lbl = tbl[:_http_parser_tr78] = new_label
 end
 lbl.set!
}
cs = 56
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
	Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_tr22]
   lbl = tbl[:_http_parser_tr22] = new_label
 end
 lbl.set!
}
cs = 57
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f5]
   lbl = tbl[:_http_parser_f5] = new_label
 end
 goto lbl
}

Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f0]
   lbl = tbl[:_http_parser_f0] = new_label
 end
 lbl.set!
}
_acts = 1
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f6]
   lbl = tbl[:_http_parser_f6] = new_label
 end
 lbl.set!
}
_acts = 3
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f7]
   lbl = tbl[:_http_parser_f7] = new_label
 end
 lbl.set!
}
_acts = 5
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f8]
   lbl = tbl[:_http_parser_f8] = new_label
 end
 lbl.set!
}
_acts = 7
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f10]
   lbl = tbl[:_http_parser_f10] = new_label
 end
 lbl.set!
}
_acts = 9
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f1]
   lbl = tbl[:_http_parser_f1] = new_label
 end
 lbl.set!
}
_acts = 11
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f2]
   lbl = tbl[:_http_parser_f2] = new_label
 end
 lbl.set!
}
_acts = 13
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f12]
   lbl = tbl[:_http_parser_f12] = new_label
 end
 lbl.set!
}
_acts = 15
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f15]
   lbl = tbl[:_http_parser_f15] = new_label
 end
 lbl.set!
}
_acts = 17
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f3]
   lbl = tbl[:_http_parser_f3] = new_label
 end
 lbl.set!
}
_acts = 19
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f14]
   lbl = tbl[:_http_parser_f14] = new_label
 end
 lbl.set!
}
_acts = 21
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f5]
   lbl = tbl[:_http_parser_f5] = new_label
 end
 lbl.set!
}
_acts = 23
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f11]
   lbl = tbl[:_http_parser_f11] = new_label
 end
 lbl.set!
}
_acts = 25
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f4]
   lbl = tbl[:_http_parser_f4] = new_label
 end
 lbl.set!
}
_acts = 28
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f9]
   lbl = tbl[:_http_parser_f9] = new_label
 end
 lbl.set!
}
_acts = 31
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f17]
   lbl = tbl[:_http_parser_f17] = new_label
 end
 lbl.set!
}
_acts = 34
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f13]
   lbl = tbl[:_http_parser_f13] = new_label
 end
 lbl.set!
}
_acts = 37
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_f16]
   lbl = tbl[:_http_parser_f16] = new_label
 end
 lbl.set!
}
_acts = 40
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser_execFuncs]
   lbl = tbl[:_http_parser_execFuncs] = new_label
 end
 lbl.set!
}
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
    Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__out]
   lbl = tbl[:_http_parser__out] = new_label
 end
 goto lbl
}	
  begin
		p += 1
		0x108804
	end

  		end
# line 44 "http11_parser.rb.rl"
# line 4329 "http11_pre.rb"
		end
	end 
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 goto lbl
}
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__again]
   lbl = tbl[:_http_parser__again] = new_label
 end
 lbl.set!
}
	if ( cs == 0 )
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__out]
   lbl = tbl[:_http_parser__out] = new_label
 end
 goto lbl
}
	end
	p += 1
	if ( p != pe )
		Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__resume]
   lbl = tbl[:_http_parser__resume] = new_label
 end
 goto lbl
}
	end
Rubinius.asm { 
 @labels = Hash.new if @labels.nil?; tbl = @labels;
 unless lbl = tbl[:_http_parser__out]
   lbl = tbl[:_http_parser__out] = new_label
 end
 lbl.set!
}
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
