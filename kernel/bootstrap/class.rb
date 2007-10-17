
class Class
  
  ivar_as_index :method_table => 1, :superclass => 6, :instance_fields => 7, :instance_flags => 8
  
  def allocate
    Ruby.primitive :allocate
    raise RuntimeError, "primitive 'allocate' failed on #{self.inspect}"
  end
  
  def new(*a)
    obj = allocate
    obj.initialize(*a)
    return obj
  end
  
  def self.new(sclass=Object, &block)
    Ruby.asm <<-ASM
#local sclass
open_class nil
    ASM
  end
  
  def instance_fields; @instance_fields ; end
  def instance_flags ; @instance_flags  ; end
  
  def instance_fields=(num)
    @instance_fields = num
  end
  
  def attr_reader(name)
    sym = "@#{name}".__symbol_lookup__
    meth = AccessVarMethod.get_ivar(sym)
    @method_table[name] = meth
    return nil
  end
  
  def attr_writer(name)
    sym = "@#{name}".__symbol_lookup__
    meth = AccessVarMethod.get_ivar(sym)
    @method_table[name] = meth
    return nil    
  end
  
  def attr_accessor(name)
    attr_reader(name)
    attr_writer(name)
    return true
  end
  
end

class RuntimePrimitive
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
    obj.put 1, RuntimePrimitive::GetIvar
    obj.put 2, 0
    obj.put 3, 0 # serial number
    obj.put 4, name
    return obj
  end
  
  def self.set_ivar(name)
    obj = allocate()
    obj.put 1, RuntimePrimitive::SetIvar
    obj.put 2, 1
    obj.put 3, 0 # serial number
    obj.put 4, name
    return obj
  end
  
  def self.get_index(idx)
    obj = allocate()
    obj.put 1, RuntimePrimitive::GetIndex
    obj.put 2, 0
    obj.put 3, 0 # serial number
    obj.put 4, idx
    return obj
  end
  
  def self.set_index(idx)
    obj = allocate()
    obj.put 1, RuntimePrimitive::SetIndex
    obj.put 2, 1
    obj.put 3, 0 # serial number
    obj.put 4, idx
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
