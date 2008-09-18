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
    filter_names :public
  end

  def private_names
    filter_names :private
  end

  def protected_names
    filter_names :protected
  end

  alias_method :to_a, :public_names

  def filter_names(filter)
    map do |name, meth|
      if meth.kind_of? RuntimePrimitive or
         (meth.kind_of?(Tuple) and meth.first == filter) then
        name
      end
    end.compact
  end
end
