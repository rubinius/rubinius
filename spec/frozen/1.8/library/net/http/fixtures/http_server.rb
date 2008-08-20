require 'webrick'
require 'webrick/httpservlet/abstract'

module NetHTTPSpecs
 class NullWriter
    def <<(s) end
    def puts(*args) end
    def print(*args) end
    def printf(*args) end
  end
  
  class RequestServlet < WEBrick::HTTPServlet::AbstractServlet
    def do_GET(req, res)
      res.content_type = "text/plain"
      res.body = "Request type: #{req.request_method}"
    end
    
    %w{ do_HEAD do_POST do_PUT do_PROPPATCH do_LOCK do_UNLOCK
        do_OPTIONS do_PROPFIND do_DELETE do_MOVE do_COPY
        do_MKCOL do_TRACE }.each do |method|
      alias_method method.to_sym, :do_GET
    end
  end
  
  class << self
    def start_server
      server_config = {
        :BindAddress => "0.0.0.0",
        :Port => 3333,
        :Logger => WEBrick::Log.new(NullWriter.new),
        :AccessLog => [],
        :ShutdownSocketWithoutClose => true,
        :ServerType => Thread }
      
      @server = WEBrick::HTTPServer.new(server_config)

      @server.mount_proc('/') do |req, res|
        res.content_type = "text/plain"
        res.body = "This is the index page."
      end
      @server.mount_proc("/form") do |req, res|
        res.content_type = "text/plain"
        res.body = req.body
      end
      @server.mount('/request', RequestServlet)

      
      @server.start
    end
    
    def stop_server
      @server.shutdown
      sleep 0.1 until @server.status == :Stop
    end
  end
end