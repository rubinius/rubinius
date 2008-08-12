class Class
  def __allocate__
    Ruby.primitive :class_allocate
    raise RuntimeError, "primitive '__allocate__' failed on #{self.inspect}"
  end

  def allocate
    __allocate__
  end

  def new(*args)
    obj = allocate()

    Rubinius.asm(args, obj) do |args, obj|
      run obj
      run args
      push_block
      send_with_splat :initialize, 0, true
      pop
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

  def put(idx, val)
    Ruby.primitive :put
    raise InvalidIndexError, "Object#put failed."
  end

  def at(idx)
    Ruby.primitive :at
    raise InvalidIndexError, "RuntimePrimitive#at failed."
  end

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
