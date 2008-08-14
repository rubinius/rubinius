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

class RuntimePrimitive < Executable
  def initialize(prim_name)
    @primitive = prim_name
    @serial = 0
  end

  def variable=(var)
    @variable = var
  end
end

class AccessVarMethod < RuntimePrimitive
  def self.get_ivar(name)
    obj = new(:get_ivar)
    obj.variable = name

    return obj
  end

  def self.set_ivar(name)
    obj = new(:set_ivar)
    obj.variable = name

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
