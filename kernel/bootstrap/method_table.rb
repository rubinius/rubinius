
##
# Method table associates symbols (names) with methods (executables). Each
# entry also contains the visibility of that method.
#

module Rubinius
  class MethodTable
    class Bucket
      attr_reader :name
      attr_reader :visibility
      attr_reader :method
      attr_reader :next
    end

    attr_reader :values
    attr_reader :bins

    def size
      @entries
    end

    alias_method :length, :size

    def self.allocate
      Ruby.primitive :methodtable_allocate
      raise PrimitiveFailure, "MethodTable.allocate primitive failed"
    end

    def initialize(hash=nil)
      return unless hash
      hash.each do |k,v|
        self[k] = v
      end
    end

    def duplicate
      Ruby.primitive :methodtable_duplicate
      raise PrimitiveFailure, "MethodTable#duplicate primitive failed"
    end

    def dup
      copy = duplicate
      Rubinius.privately do
        copy.initialize_copy self
      end
      copy
    end

    def name?(name)
      Ruby.primitive :methodtable_has_name
      raise PrimitiveFailure, "MethodTable#name? primitive failed"
    end

    alias_method :has_name?, :name?
    alias_method :include?, :name?
    alias_method :member?,  :name?

    def delete(name)
      Ruby.primitive :methodtable_delete
      raise PrimitiveFailure, "MethodTable#delete primitive failed"
    end

    def each_entry
      raise LocalJumpError, "no block given" unless block_given?

      i = 0
      while i < @bins
        if entry = @values.at(i)
          while entry
            yield entry
            entry = entry.next
          end
        end
        i += 1
      end
      self
    end


    def each
      raise LocalJumpError, "no block given" unless block_given?

      i = 0
      max = @bins
      vals = @values

      while i < max
        if entry = vals.at(i)
          while entry
            yield entry.name, entry.method, entry.visibility
            entry = entry.next
          end
        end
        i += 1
      end
      self
    end

  end
end
