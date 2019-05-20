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
      attr_reader :prediction
      attr_reader :next
    end

    attr_reader :values
    attr_reader :bins

    def size
      @entries
    end

    alias_method :length, :size

    def self.allocate
      Rubinius.primitive :methodtable_allocate
      raise PrimitiveFailure, "MethodTable.allocate primitive failed"
    end

    def initialize(hash=nil)
      return unless hash
      hash.each do |k, v|
        self[k] = v
      end
    end

    private :initialize

    def duplicate
      Rubinius.primitive :methodtable_duplicate
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
      Rubinius.primitive :methodtable_has_name
      raise PrimitiveFailure, "MethodTable#name? primitive failed"
    end

    alias_method :has_name?, :name?
    alias_method :include?, :name?
    alias_method :member?,  :name?

    def delete(name)
      Rubinius.primitive :methodtable_delete
      raise PrimitiveFailure, "MethodTable#delete primitive failed"
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
##
# Holds Executables for lookup by the VM.
#
# When looking up an Executable, Rubinius starts at the real class (the object
# in the class slot of the receiver) and looks in its MethodTable for the
# method name.  If the method is not found the ancestors list will be
# walked looking for the method.
#
# If no method was found, Rubinius restarts at the real class looking for
# "method_missing", walking the ancestors list if it is not found.
#
# If "method_missing" is not found, a VM assertion is triggered.

module Rubinius
  class MethodTable
    include Enumerable

    def filter_entries
      out = []
      i = 0
      while i < @bins
        if entry = @values.at(i)
          while entry
            if val = yield(entry)
              out << val
            end

            entry = entry.next
          end
        end
        i += 1
      end

      out
    end

    def public_names
      filter_entries do |entry|
        entry.visibility == :public ? entry.name : nil
      end
    end

    def private_names
      filter_entries do |entry|
        entry.visibility == :private ? entry.name : nil
      end
    end

    def protected_names
      filter_entries do |entry|
        entry.visibility == :protected ? entry.name : nil
      end
    end

    alias_method :to_a, :public_names
  end
end
