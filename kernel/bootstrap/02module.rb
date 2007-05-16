class Module
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def methods; Ruby.asm "push self\npush 1\nfetch_field"; end
  def method_cache; Ruby.asm "push self\npush 2\nfetch_field"; end
  def name; Ruby.asm "push self\npush 3\nfetch_field"; end
  def constants; Ruby.asm "push self\npush 4\nfetch_field"; end
  def parent; Ruby.asm "push self\npush 5\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :methods => 1, :method_cache => 2, :name => 3, :constants => 4, :parent => 5
  
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
    cls.superclass = self
  end

  def superclass
    @superclass
  end
  
  def direct_superclass
    @superclass
  end
  
end
  
