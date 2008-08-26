# Defines basic entities required to load bootstrap/hash.rb
# and common/hash.rb in MRI.

# Silence all the method redefined warnings
$VERBOSE = nil

Tuple = Array

class Tuple
  alias_method :put, :[]=
end

Undefined = Object.new

class Hash
  def self.new(default = Undefined, &block)
    h = allocate
    h.send :initialize, default, &block
    h
  end
end
