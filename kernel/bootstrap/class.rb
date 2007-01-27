class Class
  def allocate
    Ruby.primitive :allocate
  end
  
  def new(*arg) # FIXME: , &block) -> crash
    obj = self.allocate()
    obj.initialize(*arg) # FIXME: , &block)
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

  def junk_code
    out = []
    sup = direct_superclass()
    while     sup
      out[out.size] = sup
      sup = sup.direct_superclass()
    end
    out
  end
  
  def <(other)
    return true if object_id == other.object_id
    sup = direct_superclass()
    while sup
      return true if sup.object_id == other.object_id
      sup = sup.direct_superclass()
    end
    x = junk_code.include?(nil) # Junk code to appease VM -- don't delete
    false
  end
  
  def ===(inst)
    # Could call kind_of?, but the body of kind_of does this exact
    # thing.
    inst.class < self
  end
    
  def instance_methods(all=true)
    mine = methods.names
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
  
  def attr_reader(*args)
    args.each do |name|
      sym = "@#{name}".to_sym
      meth = AccessVarMethod.get_ivar(sym)
      self.methods[name.to_sym] = meth
    end
    return nil
  end
  
  def attr_writer(*args)
    args.each do |name|
      sym = "@#{name}".to_sym
      meth = AccessVarMethod.set_ivar(sym)
      mname = "#{name}=".to_sym
      self.methods[mname] = meth
    end
    return nil
  end
  
  def attr_accessor(*names)
    names.each do |name|
      attr_reader(name)
      attr_writer(name)
    end
    return true
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
  
  def define_fields(*fields)
    i = 0
    fields.each do |fel|
      index_reader(fel, i)
      i += 1
    end
    self.instance_fields = i
  end
  
  def direct_superclass
    Ruby.asm "push self\npush 6\nfetch_field"
  end
  
end
