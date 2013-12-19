module Rubinius
  class MonoInlineCache < CallSite

    attr_reader :name
    attr_reader :executable
    attr_reader :receiver_class
    attr_reader :stored_module
    attr_reader :method

    def hits
      Rubinius.primitive :mono_inline_cache_hits
      raise PrimitiveFailure, "MonoInlineCache#hits primitive failed"
    end

    def method_missing
      Rubinius.primitive :mono_inline_cache_method_missing
      raise PrimitiveFailure, "MonoInlineCache#method_missing primitive failed"
    end

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} #{location}##{@name}(#{hits}) receiver_class=#{@receiver_class} stored_module=#{@stored_module} method=#{@method.inspect}>"
    end

  end
end
