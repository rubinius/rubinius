module Rubinius
  class RespondToCache < CallSite

    attr_reader :name
    attr_reader :executable
    attr_reader :receiver_class
    attr_reader :message
    attr_reader :visibility
    attr_reader :responds
    attr_reader :fallback_call_site

    def hits
      Rubinius.primitive :respond_to_cache_hits
      raise PrimitiveFailure, "RespondToCache#hits primitive failed"
    end

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} #{location}##{@name}(#{hits}) receiver_class=#{@receiver_class} message=#{@message} visibility=#{@visibility} responds=#{@responds}>"
    end

  end
end
