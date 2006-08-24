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
  
  def alias_method(nw,cur)
    meth = methods[cur]
    unless meth
      exc = NoMethodError.new("No method by the name of '#{cur}' for #{self}")
      raise exc
    end
    
    methods[nw] = meth
    return meth
  end
  
  def instance_methods
    methods.keys
  end
  
  def superclass
    cls = direct_superclass()
    return nil unless cls
    while cls and cls.class == IncludedModule
      cls = cls.direct_superclass()
    end
    return cls
  end
  
  def direct_superclass
    Ruby.asm "push self\npush 4\nfetch_field"
  end
  
  def instance_fields=(num)
    put 5, num
  end
end