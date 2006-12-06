class Module
  def name
    at(2)
  end
  
  def include(mod)
    im = IncludedModule.new(mod)
    im.attach_to self
  end
end

class IncludedModule < Module
  self.instance_fields = 8

  def initialize(mod)
    put 0, mod.instance_variables
    put 1, mod.methods
    put 2, mod.name
    put 7, mod
  end

  def old_to_s
    "#<IM>"
  end

  def module
    at(7)
  end

  def attach_to(cls)
    put 6, cls.superclass
    cls.put 6, self    
  end

  def superclass
    at(6)
  end

  def direct_superclass
    Ruby.asm "push self\npush 6\nfetch_field"
  end

end
  
