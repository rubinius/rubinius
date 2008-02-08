
class Class
  
  ivar_as_index :method_table => 1, :superclass => 6, :instance_fields => 7, :has_ivars => 8, :needs_cleanup => 9, :object_type => 10
  
  def allocate
    Ruby.primitive :allocate
    raise RuntimeError, "primitive 'allocate' failed on #{self.inspect}"
  end
  
  def new(*args)
    obj = allocate()
    Rubinius.asm(args, obj) do |args, obj|
      run args
      cast_array_for_args 0
      push_array
      push_block
      run obj
      set_call_flags 1
      send_with_register :initialize
    end

    return obj
  end
  
  def instance_fields; @instance_fields ; end
  def instance_flags ; @instance_flags  ; end
  
  def instance_fields=(num)
    @instance_fields = num
  end
  
  def object_type=(type)
    @object_type = type
  end
  
  def object_type
    @object_type
  end

  def has_ivars
    @has_ivars
  end

  def needs_cleanup
    @needs_cleanup
  end
end

class RuntimePrimitive

  PrimitiveIndex = 1
  RequiredArguments = 2
  SerialNumber = 3
  ByteCodes = 4

end

class AccessVarMethod < RuntimePrimitive
  self.instance_fields = 5

  def self.get_ivar(name)
    obj = allocate()
    obj.put RuntimePrimitive::PrimitiveIndex, :get_ivar
    obj.put RuntimePrimitive::RequiredArguments, 0
    obj.put RuntimePrimitive::SerialNumber, 0 # serial number
    obj.put RuntimePrimitive::ByteCodes, name
    return obj
  end
  
  def self.set_ivar(name)
    obj = allocate()
    obj.put RuntimePrimitive::PrimitiveIndex, :set_ivar
    obj.put RuntimePrimitive::RequiredArguments, 1
    obj.put RuntimePrimitive::SerialNumber, 0 # serial number
    obj.put RuntimePrimitive::ByteCodes, name
    return obj
  end
  
  def self.get_index(idx)
    obj = allocate()
    obj.put RuntimePrimitive::PrimitiveIndex, :get_index
    obj.put RuntimePrimitive::RequiredArguments, 0
    obj.put RuntimePrimitive::SerialNumber, 0 # serial number
    obj.put RuntimePrimitive::ByteCodes, idx
    return obj
  end
  
  def self.set_index(idx)
    obj = allocate()
    obj.put RuntimePrimitive::PrimitiveIndex, :set_index
    obj.put RuntimePrimitive::RequiredArguments, 1
    obj.put RuntimePrimitive::SerialNumber, 0 # serial number
    obj.put RuntimePrimitive::ByteCodes, idx
    return obj
  end
end

# This is here because it uses instance_fields, which is defined above.
# This is the ONLY way to force load order in the bootstrap (and the 
# only instance of load order mattering)
class IncludedModule < Module
  self.instance_fields = 8

  # HACK: make this a VM exported constant
  self.object_type = 7

  ivar_as_index :__ivars__ => 0, :method_table => 1, :method_cache => 2, :name => 3, :constants => 4, :encloser => 5, :superclass => 6, :module => 7
  def superclass; @superclass ; end
  def module    ; @module     ; end

  def initialize(mod)
    @method_table = mod.method_table
    @method_cache = nil
    @name = nil
    @constants = mod.constant_table
    @encloser = mod.encloser
    @module = mod
  end

  def attach_to(cls)
    @superclass = cls.direct_superclass
    cls.superclass = self
  end

  def name
    @module.name
  end

  def to_s
    @module.to_s
  end
end
