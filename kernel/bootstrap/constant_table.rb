# -*- encoding: us-ascii -*-

module Rubinius
  class ConstantTable
    class Bucket
      attr_reader :name
      attr_accessor :constant
      attr_accessor :visibility
      attr_reader :next

      def inspect
        "#<#{self.class}:0x#{object_id.to_s(16)} name=#{@name.inspect} constant=#{@constant.inspect} visibility=#{visibility.inspect}>"
      end
    end

    attr_reader :values
    attr_reader :bins

    def size
      @entries
    end

    alias_method :length, :size

    def self.allocate
      Rubinius.primitive :constant_table_allocate
      raise PrimitiveFailure, "ConstantTable.allocate primitive failed"
    end

    def initialize(hash=nil)
      return unless hash
      hash.each do |k, v|
        self[k] = v
      end
    end

    def duplicate
      Rubinius.primitive :constant_table_duplicate
      raise PrimitiveFailure, "ConstantTable#duplicate primitive failed"
    end

    def dup
      copy = duplicate
      Rubinius.privately do
        copy.initialize_copy self
      end
      copy
    end

    def name?(name)
      Rubinius.primitive :constant_table_has_name
      raise PrimitiveFailure, "ConstantTable#name? primitive failed"
    end

    alias_method :has_name?, :name?
    alias_method :include?, :name?
    alias_method :member?,  :name?

    def keys
      Rubinius.primitive :constant_table_keys
      raise PrimitiveFailure, "ConstantTable#keys primitive failed"
    end

    def delete(name)
      Rubinius.primitive :constant_table_delete
      raise PrimitiveFailure, "ConstantTable#delete primitive failed"
    end

    def each
      raise LocalJumpError, "no block given" unless block_given?

      i = 0
      max = @bins
      vals = @values

      while i < max
        if entry = vals.at(i)
          while entry
            yield entry.name, entry.constant, entry.visibility
            entry = entry.next
          end
        end
        i += 1
      end
      self
    end

  end
end

