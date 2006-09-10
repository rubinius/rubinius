class Class
  def allocate
    Ruby.primitive :allocate
  end
  
  def new(*arg)
    obj = self.allocate()
    obj.initialize(*arg)
    return obj
  end
  
  def ancestors
    out = [self]
    sup = direct_superclass()
    while sup
      out << sup
      sup = sup.direct_superclass()
    end
    return out
  end
  
  def <(other)
    ancestors.include?(other)
  end
  
  def ===(inst)
    # Could call kind_of?, but the body of kind_of does this exact
    # thing.
    inst.class < self
  end
    
  def instance_methods(all=true)
    mine = methods.keys
    return mine unless all
    sup = direct_superclass()
    while sup
      sup.methods.keys.each do |e|
        mine << e
      end
      sup = sup.direct_superclass()
    end
    
    return mine
  end
  
  def superclass
    cls = direct_superclass()
    return nil unless cls
    while cls and cls.class == IncludedModule
      cls = cls.direct_superclass()
    end
    return cls
  end
  
  def attr_reader(name)
    sym = "@#{name}".to_sym
    meth = AccessVarMethod.get_ivar(sym)
    self.methods[name.to_sym] = meth
    return name
  end
  
  def attr_writer(name)
    sym = "@#{name}".to_sym
    meth = AccessVarMethod.set_ivar(sym)
    mname = "#{name}=".to_sym
    self.methods[mname] = meth
    return name
  end
  
  def attr_accessor(name)
    attr_reader(name)
    attr_writer(name)
    return name
  end
  
  def index_reader(name, idx)
    meth = AccessVarMethod.get_index(idx)
    self.methods[name.to_sym] = meth
    return name
  end
  
  def index_writer(name, idx)
    meth = AccessVarMethod.set_index(idx)
    mname = "#{name}="
    self.methods[mname.to_sym] = meth
    return name
  end
  
  def index_accessor(name, idx)
    index_reader(name, idx)
    index_writer(name, idx)
  end
  
  def direct_superclass
    Ruby.asm "push self\npush 4\nfetch_field"
  end
  
end