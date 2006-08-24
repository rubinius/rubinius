class Module
  def name
    at(2)
  end
  
  def to_s
    name.to_s
  end
  
  def include(mod)
    im = IncludedModule.new(mod)
    im.attach_to self
  end
end

class IncludedModule < Module
  self.instance_fields = 6
  
  def initialize(mod)
    put 0, mod.instance_variables
    put 1, mod.methods
    put 2, mod.name
    put 5, mod
  end
  
  def old_to_s
    "#<IM>"
  end
  
  def module
    at(5)
  end
  
  def attach_to(cls)
    put 4, cls.superclass
    cls.put 4, self    
  end
  
  def superclass
    at(4)
  end
  
  def direct_superclass
    Ruby.asm "push self\npush 4\nfetch_field"
  end
  
end

class Object
  include Kernel
end