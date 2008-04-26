module Mongrel
  class HttpParserError < IOError; end
  
%%{
  machine http_parser;

  action mark { @mark = fpc }
  action start_field { @field_start = fpc }
  action snake_upcase_field { fpc }
  action write_field { @field_len = fpc - @field_start }
  action start_value { @mark = fpc }  
  action write_value {
    if params.respond_to? :[]=
      field = data.slice(@field_start, @field_len).split('-').map { |f| f.upcase }.join('_')
      value = data.slice(@mark, fpc - @mark)
      
      params[HTTP_PREFIX + field] = value
    end
  }
  action request_method {
    params['REQUEST_METHOD'] = data.slice @mark, fpc - @mark
  }
  action request_uri {
    params['REQUEST_URI'] = data.slice @mark, fpc - @mark
  }
  action fragment {
    params['FRAGMENT'] = data.slice @mark, fpc - @mark
  }
  
  action start_query { @query_start = fpc }
  
  action query_string {
    params['QUERY_STRING'] = data.slice @query_start, fpc - @query_start
  }

  action http_version {
    params['SERVER_PROTOCOL'] = data.slice @mark, fpc - @mark
  }

  action request_path {
    params['REQUEST_PATH'] = data.slice @mark, fpc - @mark
  }

  action done {
    params['SERVER_SOFTWARE']   = SERVER_SOFTWARE
    params['SERVER_PROTOCOL']   = SERVER_PROTOCOL
    params['GATEWAY_INTERFACE'] = GATEWAY_INTERFACE
    
    params['CONTENT_LENGTH'] = params['HTTP_CONTENT_LENGTH'] if params['HTTP_CONTENT_LENGTH']
    params['CONTENT_TYPE']   = params['HTTP_CONTENT_TYPE']   if params['HTTP_CONTENT_TYPE']

    if params['HTTP_HOST']
      params['SERVER_NAME'], params['SERVER_PORT'] = params['HTTP_HOST'].split ':'
      params['SERVER_PORT'] ||= '80'
    end
    
    @body_start = fpc + 1    
    params.http_body = data.slice(fpc + 1, pe - fpc - 1) if params.respond_to? :http_body=
    fbreak;
  }

  include http_parser_common "http11_parser_common.rl";
}%%

  class HttpParser  
    SERVER_SOFTWARE   = 'Mongrel (Rubinius)'
    SERVER_PROTOCOL   = 'HTTP/1.1'
    GATEWAY_INTERFACE = 'CGI/1.2'
    HTTP_PREFIX       = 'HTTP_'
    
    %% write data;    
    
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

      %% write exec;

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

    def reset
      @cs = http_parser_start
    
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
  end
end
