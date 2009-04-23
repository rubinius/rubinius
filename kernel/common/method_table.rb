# depends on: class.rb

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
  class MethodTable < LookupTable

    def []=(name, val)
      if val.kind_of? Executable or
         val.kind_of? CompiledMethod::Visibility or
         val == false or
         val == nil
        super(name, val)
      else
        raise ArgumentError, "Invalid method table entry class: #{val.class}"
      end
    end

    def public_names
      select { |n, m| m && m.public? }.map! { |n, m| n }
    end

    def private_names
      select { |n, m| m && m.private? }.map! { |n, m| n }
    end

    def protected_names
      select { |n, m| m && m.protected? }.map! { |n, m| n }
    end

    alias_method :to_a, :public_names
  end
end
