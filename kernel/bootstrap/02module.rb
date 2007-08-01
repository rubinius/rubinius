class Module
  ivar_as_index :__ivars__ => 0, :methods => 1, :method_cache => 2, :name => 3, :constants => 4, :parent => 5
  def __ivars__      ; @__ivars__    ; end
  # Do not define #methods as an accessor for @methods, use #method_table instead
  def method_cache   ; @method_cache ; end
  def name           ; @name         ; end
  def constants_table; @constants    ; end
  def parent         ; @parent       ; end
  
  def method_table
    @methods
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
  
  def find_method_in_hierarchy(sym)
    [self, Functions].each do |mod|
      meth = mod.method_table[sym]
      return meth if meth
    end
    nil
  end

  def alias_method(new_name, current_name)
    meth = find_method_in_hierarchy(current_name)
    if meth
      method_table[new_name] = meth
    else
      raise NameError, "undefined method `#{current_name}' for module `#{self.name}'"
    end
  end
end

class IncludedModule < Module
  self.instance_fields = 8

  ivar_as_index :superclass => 6, :module => 7
  def superclass; @superclass ; end
  def module    ; @module     ; end

  def initialize(mod)
    @__ivars__ = mod.__ivars__
    @methods = mod.method_table
    @method_cache = nil
    @name = mod.name
    @constants = {}
    @parent = nil
    @module = mod
  end

  def old_to_s
    "#<IM>"
  end

  def attach_to(cls)
    @superclass = cls.direct_superclass
    cls.superclass = self
  end

  def direct_superclass
    @superclass
  end
end
  
