module Rubinius
  class ConstantCache
    attr_reader :name
    attr_reader :value
    attr_reader :module
    attr_reader :lexical_scope
    attr_reader :executable

    def ip
      Rubinius.primitive :constant_cache_ip
      raise PrimitiveFailure, "CallSite#ip primitive failed"
    end

    def serial
      Rubinius.primitive :constant_cache_serial
      raise PrimitiveFailure, "CallSite#serial primitive failed"
    end

    def location
      "#{@executable.file}:#{@executable.line_from_ip(ip)}"
    end

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} #{location}##{@name} constant=#{@value} module=#{@module} lexical_scope=#{@lexical_scope}>"
    end
  end
end

