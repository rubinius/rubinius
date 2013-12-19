module Rubinius
  class CallSite

    attr_reader :name
    attr_reader :executable

    def hits
      0
    end

    def ip
      Rubinius.primitive :call_site_ip
      raise PrimitiveFailure, "CallSite#ip primitive failed"
    end

    def location
      "#{@executable.file}:#{@executable.line_from_ip(ip)}"
    end

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} #{location}##{@name}(#{hits})>"
    end

  end
end
