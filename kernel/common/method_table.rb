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
