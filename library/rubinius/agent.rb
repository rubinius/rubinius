require 'rubinius/bert'

module Rubinius
  class Agent
    def self.connect(host, port, &b)
      i = new TCPSocket.new(host, port)
      i.handshake!(&b)
      return i
    end

    def self.loopback
      new Rubinius.agent_loopback
    end

    def initialize(io, password=nil)
      @io = io
      @decoder = BERT::Decode.new(@io)
      @encoder = BERT::Encode.new(@io)
      @password = password
    end

    def handshake!
      @handshake = @decoder.read_any
      if @handshake[0] == :file_auth
        File.open(@handshake[1], "w") do |f|
          f.puts "agent start"
        end

        @encoder.write_any :ok

        @handshake = @decoder.read_any
      elsif @handshake[0] == :password_auth
        if @password
          @encoder.write_any t[:password, @password]
        elsif block_given?
          password = yield
          @encoder.write_any t[:password, password.to_s]
        else
          raise "Password required, none available"
        end

        @handshake = @decoder.read_any
      end

      unless @handshake[0] == :hello_query_agent
        raise "Error receiving hello: #{@handshake.inspect}"
      end
    end

    attr_reader :handshake

    def request(*args)
      @encoder.write_any t[*args]
      @decoder.read_any
    end

    class GetError < RuntimeError
      def initialize(var, code, msg)
        super("#{code} - #{msg} (#{var})")
      end
    end

    def get(var)
      @encoder.write_any t[:get_config, var]
      code, kind, val = @decoder.read_any

      if code != :ok
        raise GetError.new(var, code, kind)
      end

      return [kind, val]
    end

    def close
      begin
        @encoder.write_any :close
        response = @decoder.read_any
      rescue Errno::EPIPE, IOError
      end

      response
    ensure
      begin
        @io.close
      rescue IOError
      end
    end

    def bind(port)
      @encoder.write_any t[:bind, Integer(port)]
      @decoder.read_any
    end
  end
end
