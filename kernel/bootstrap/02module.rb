class Module
  def name
    @name
  end
  
  def append_features(mod)
    im = IncludedModule.new(self)
    im.attach_to mod
  end
  
  def included(mod); end
  
  def include(mod)
    mod.append_features(self)
    mod.included(self)
  end
  
  def alias_method(nw,cur)
    meth = @methods[cur]
    unless meth
      raise NoMethodError, "No method by the name of '#{cur}' for #{self}"
    end
    
    @methods[nw] = meth
    return meth
  end

end

class IncludedModule < Module
  self.instance_fields = 8

  ivar_as_index :superclass => 6, :module => 7

  def initialize(mod)
    @__ivars__ = mod.__ivars__
    @methods = mod.methods
    @method_cache = nil
    @name = mod.name
    @constants = {}
    @parent = nil
    @module = mod
  end

  def old_to_s
    "#<IM>"
  end

  def module
    @module
  end

  def attach_to(cls)
    @superclass = cls.superclass
    cls.put 6, self    
  end

  def superclass
    @superclass
  end
  
  def direct_superclass
    @superclass
  end
  
end
  
