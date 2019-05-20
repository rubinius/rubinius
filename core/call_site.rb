module Rubinius
  class CallSite
    attr_reader :name
    attr_reader :cache

    def ip
      Rubinius.primitive :call_site_ip
      raise PrimitiveFailure, "CallSite#ip primitive failed"
    end

    def size
      Rubinius.primitive :call_site_size
      raise PrimitiveFailure, "CallSite#size primitive failed"
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

    def evictions
      Rubinius.primitive :call_site_evictions
      raise PrimitiveFailure, "CallSite#evictions primitive failed"
    end

    def rotations
      Rubinius.primitive :call_site_rotations
      raise PrimitiveFailure, "CallSite#rotations primitive failed"
    end

    def dead_list_size
      Rubinius.primitive :call_site_dead_list_size
      raise PrimitiveFailure, "CallSite#dead_list_size primitive failed"
    end

    def cache_entries
      Rubinius.primitive :call_site_cache_entries
      raise PrimitiveFailure, "CallSite#caches primitive failed"
    end

    def reset
      Rubinius.primitive :call_site_reset
      raise PrimitiveFailure, "CallSite#reset primitive failed"
    end

    def inspect
      str = "#<#{self.class.name}:0x#{self.object_id.to_s(16)} name=#{@name} ip=#{ip} " \
            "size=#{size} invokes=#{invokes} hits=#{hits} misses=#{misses} " \
            "evictions=#{evictions} rotations=#{rotations} dead_list=#{dead_list_size}"

      cache_entries.each do |entry|
        str << " #<Entry"
        i = 0
        while i < entry.size
          str << " #{entry[i]}=#{entry[i+1]}"
          i += 2
        end
        str << ">"
      end

      str << ">"
    end

    alias_method :to_s, :inspect
  end
end
