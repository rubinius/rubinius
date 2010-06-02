require 'rubinius/bert'

module Rubinius
  class Agent
    def self.connect(host, port)
      i = new TCPSocket.new(host, port)
      i.handshake!
      return i
    end

    def self.loopback
      new Rubinius.agent_io
    end

    def initialize(io)
      @io = io
      @decoder = BERT::Decode.new(@io)
      @encoder = BERT::Encode.new(@io)
    end

    def handshake!
      @handshake = @decoder.read_any
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
