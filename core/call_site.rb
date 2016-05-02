module Rubinius
  class CallSite
    attr_reader :name
    attr_reader :cache

    def ip
      Rubinius.primitive :call_site_ip
      raise PrimitiveFailure, "CallSite#ip primitive failed"
    end

    def depth
      Rubinius.primitive :call_site_depth
      raise PrimitiveFailure, "CallSite#depth primitive failed"
    end

    def invokes
      Rubinius.primitive :call_site_invokes
      raise PrimitiveFailure, "CallSite#invokes primitive failed"
    end

    def hits
      Rubinius.primitive :call_site_hits
      raise PrimitiveFailure, "CallSite#hits primitive failed"
    end

    def misses
      Rubinius.primitive :call_site_misses
      raise PrimitiveFailure, "CallSite#misses primitive failed"
    end

    def reset
      Rubinius.primitive :call_site_reset
      raise PrimitiveFailure, "CallSite#reset primitive failed"
    end

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} name=#{@name} ip=#{ip} depth=#{depth} invokes=#{invokes} hits=#{hits} misses=#{misses}>"
    end
  end
end
