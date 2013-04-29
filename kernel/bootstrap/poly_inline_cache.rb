# -*- encoding: us-ascii -*-

module Rubinius

  class InlineCacheEntry

    attr_reader :receiver_class
    attr_reader :stored_module
    attr_reader :method

    def hits
      Rubinius.primitive :inline_cache_entry_hits
      raise PrimitiveFailure, "InlineCacheEntry#hits primitive failed"
    end

    def method_missing
      Rubinius.primitive :inline_cache_entry_method_missing
      raise PrimitiveFailure, "InlineCacheEntry#method_missing primitive failed"
    end

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} receiver_class=#{@receiver_class} stored_module=#{@stored_module} method=#{@method.inspect}>"
    end
  end

  class PolyInlineCache < CallSite

    attr_reader :name
    attr_reader :executable

    def entries
      Rubinius.primitive :poly_inline_cache_entries
      raise PrimitiveFailure, "PolyInlineCache#entries primitive failed"
    end

    def overflows
      Rubinius.primitive :poly_inline_cache_overflows
      raise PrimitiveFailure, "PolyInlineCache#overflows primitive failed"
    end

    def hits
      count = 0
      entries.each do |entry|
        count += entry.hits if entry
      end
      count
    end

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} #{location}##{@name}(#{hits}) overflows=#{overflows} entries=#{entries}>"
    end

  end
end
