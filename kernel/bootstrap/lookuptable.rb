##
# A LookupTable is similar to a Hash in that keys are used to set and
# reference values. However, unlike Hash, whether a key matches an
# entry in LookupTable is determined by using the == comparison operator
# in C code. In effect, two keys are equal if they are the same pointer.
#
# NOTE: the value used to determine the bin is the "pointer" value of
# key >> 2. This makes it possible to calculate the bin from Ruby with
# the same result as in C. For example:
# 
#   l = LookupTable.new
#   loc = :a.hash & (l.bins - 1)  # => 12
#   class LookupTable
#     def show
#       @values
#     end
#   end
#
#   l[:a] = 1
#   l.show  # => #<Tuple: nil, ..., nil, #<Bucket: @key=>:a, @value=>1,
#                         @next=>nil>, nil, nil, nil>
#
# where ... is ten "nil, " entries.
#
# LookupTable is intended to be used with Symbol or Fixnum keys because
# it is a strict identity map. Usage of Strings as keys will likely result
# in strange behavior.
# LookupTable is NOT intended to be used generally like Hash.

module Rubinius
  class LookupTable
    class Bucket
      attr_reader :key
      attr_reader :value
      attr_reader :next
    end

    attr_reader :values
    attr_reader :bins

    attr_reader_specific :entries, :size
    alias_method :length, :size

    def self.allocate
      Ruby.primitive :lookuptable_allocate
      raise PrimitiveFailure, "LookupTable.allocate primitive failed"
    end

    def initialize(hash=nil)
      return unless hash
      hash.each do |k,v|
        self[k] = v
      end
    end

    private :initialize

    def duplicate
      Ruby.primitive :lookuptable_duplicate
      raise PrimitiveFailure, "LookupTable#duplicate primitive failed"
    end

    def dup
      copy = duplicate
      Rubinius.privately do
        copy.initialize_copy self
      end
      copy
    end

    alias_method :clone, :dup

    def fetch(key, return_on_failure)
      Ruby.primitive :lookuptable_fetch
      raise PrimitiveFailure, "fetch failed"
    end

    def key?(key)
      Ruby.primitive :lookuptable_has_key
      raise PrimitiveFailure, "LookupTable#key? primitive failed"
    end

    alias_method :has_key?, :key?
    alias_method :include?, :key?
    alias_method :member?,  :key?

    def delete(key)
      Ruby.primitive :lookuptable_delete
      raise PrimitiveFailure, "LookupTable#delete primitive failed"
    end

    def keys
      Ruby.primitive :lookuptable_keys
      raise PrimitiveFailure, "LookupTable#keys primitive failed"
    end

    def values
      Ruby.primitive :lookuptable_values
      raise PrimitiveFailure, "LookupTable#keys primitive failed"
    end

    def each
      max = @bins
      i = 0
      vals = @values

      while i < max
        entry = vals.at(i)

        while entry
          yield entry.key, entry.value
          entry = entry.next
        end
        i += 1
      end
      self
    end

    alias_method :each_entry, :each

    def empty?
      @entries == 0
    end
  end
end
