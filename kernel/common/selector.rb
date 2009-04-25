##
# The representation of a method name used for invalidating a cached SendSite.

module Rubinius
  class Selector
    attr_reader :name
    attr_reader :send_sites

    def inspect
      "#<#{self.class}:0x#{object_id.to_s(16)} name=#{@name} sites=#{@send_sites.size}>"
    end

    def receives
      @send_sites.inject(0) { |acc, ss| acc + ss.hits + ss.misses }
    end
  end
end
