
class Class
  
  ivar_as_index :method_table => 1, :superclass => 6, :instance_fields => 7, :instance_flags => 8
  
  def allocate
    Ruby.primitive :allocate
    raise RuntimeError, "primitive 'allocate' failed on #{self.inspect}"
  end
  
  def new(*args)
    obj = allocate()
    Ruby.asm <<-CODE
#local args
cast_array_for_args 0
push_array
push_block
#local obj
set_call_flags 1
&send initialize +
    CODE

    return obj
  end
  
  def instance_fields; @instance_fields ; end
  def instance_flags ; @instance_flags  ; end
  
  def instance_fields=(num)
    @instance_fields = num
  end
end

class RuntimePrimitive

  PrimitiveIndex = 1
  RequiredArguments = 2
  SerialNumber = 3
  ByteCodes = 4

  SetIvar = 1024
  GetIvar = 1025
  SetIndex = 1026
  GetIndex = 1027
  DispatchAsMethod = 1028
end

class AccessVarMethod < RuntimePrimitive
  self.instance_fields = 5
  
  def self.get_ivar(name)
    obj = allocate()
    obj.put RuntimePrimitive::PrimitiveIndex, RuntimePrimitive::GetIvar
    obj.put RuntimePrimitive::RequiredArguments, 0
    obj.put RuntimePrimitive::SerialNumber, 0 # serial number
    obj.put RuntimePrimitive::ByteCodes, name
    return obj
  end
  
  def self.set_ivar(name)
    obj = allocate()
    obj.put RuntimePrimitive::PrimitiveIndex, RuntimePrimitive::SetIvar
    obj.put RuntimePrimitive::RequiredArguments, 1
    obj.put RuntimePrimitive::SerialNumber, 0 # serial number
    obj.put RuntimePrimitive::ByteCodes, name
    return obj
  end
  
  def self.get_index(idx)
    obj = allocate()
    obj.put RuntimePrimitive::PrimitiveIndex, RuntimePrimitive::GetIndex
    obj.put RuntimePrimitive::RequiredArguments, 0
    obj.put RuntimePrimitive::SerialNumber, 0 # serial number
    obj.put RuntimePrimitive::ByteCodes, idx
    return obj
  end
  
  def self.set_index(idx)
    obj = allocate()
    obj.put RuntimePrimitive::PrimitiveIndex, RuntimePrimitive::SetIndex
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

  ivar_as_index :superclass => 6, :module => 7
  def superclass; @superclass ; end
  def module    ; @module     ; end

  def initialize(mod)
    @methods = mod.method_table
    @method_cache = nil
    @name = mod.name.__symbol_lookup__
    @constants = mod.constant_table
    @parent = mod.parent
    @module = mod
  end

  def attach_to(cls)
    @superclass = cls.direct_superclass
    cls.superclass = self
  end

end
