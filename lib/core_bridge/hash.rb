# Defines basic entities required to load bootstrap/hash.rb
# and common/hash.rb in MRI.

# Silence all the method redefined warnings
$VERBOSE = nil

Undefined = Object.new

class Object
  def set_instance_variable(sym, value)
    instance_variable_set sym, value
  end
end

class Hash
  def self.new(default = Undefined, &block)
    h = allocate
    h.send :initialize, default, &block
    h
  end

  # This is overwritten in kernel/common/hash.rb but it is
  # helpful to have it here while building up Hash methods.
  def initialize(default, &block)
  end
end
