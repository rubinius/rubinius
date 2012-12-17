# -*- encoding: us-ascii -*-

module Rubinius
module FFI

  # Represents a C enum.
  class Enum

    attr_reader :tag
    attr_reader :kv_map

    def initialize(info, tag=nil)
      @tag = tag
      @kv_map = Hash.new

      unless info.nil?
        last_cst = nil
        value = 0
        info.each do |i|
          case i
          when Symbol
            @kv_map[i] = value
            last_cst = i
            value += 1
          when Integer
            @kv_map[last_cst] = i
            value = i+1
          end

        end

      end

    end

    def inspect
      "#<%s:0x%x %s=>%s>" % [self.class,self.object_id,@tag,@kv_map.inspect]
    end

    def anonym?
      @tag.nil?
    end

    def values
      @kv_map.values
    end

    def symbols
      @kv_map.keys
    end

    def [](sym)
      @kv_map[sym]
    end

    def symbol(value)
      key,val = @kv_map.detect { |key,val| val==value }
      return key
    end

  end
end
end
