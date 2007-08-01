class Class
  def allocate
    Ruby.primitive :allocate
    raise RuntimeError, "primitive 'allocate' failed on #{self.inspect}"
  end
  
  # A version taking a block is available in core; it's too early to
  # support one during bootstrap
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
  
  def find_method_in_hierarchy(sym)
    mod = self
    while mod
      meth = mod.method_table[sym]
      if meth
        return meth
      end
      
      if mod == Functions
        mod = nil
      else
        mod = mod.direct_superclass || Functions
      end
    end
    nil
  end
  
  def alias_method(new_name, current_name)
    meth = find_method_in_hierarchy(current_name)
    if meth
      method_table[new_name] = meth
    else
      raise NameError, "undefined method `#{current_name}' for class `#{self.name}'"
    end
  end

  def <(other)
    return true if object_id == other.object_id
    sup = direct_superclass()
    while sup
      return true if sup.object_id == other.object_id
      sup = sup.direct_superclass()
    end
    false
  end
  
  def ===(inst)
    # Could call kind_of?, but the body of kind_of does this exact
    # thing.
    inst.kind_of? self
  end
    
  def superclass
    cls = direct_superclass
    return nil unless cls
    while cls and cls.class == IncludedModule
      cls = cls.direct_superclass
    end
    return cls
  end
  
  def attr_reader(*args)
    args.each do |name|
      sym = "@#{name}".to_sym
      meth = AccessVarMethod.get_ivar(sym)
      self.method_table[name.to_sym] = meth
    end
    return nil
  end
  
  def attr_writer(*args)
    args.each do |name|
      sym = "@#{name}".to_sym
      meth = AccessVarMethod.set_ivar(sym)
      mname = "#{name}=".to_sym
      self.method_table[mname] = meth
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
    self.method_table[name.to_sym] = meth
    return name
  end
  
  def index_writer(name, idx)
    meth = AccessVarMethod.set_index(idx)
    mname = "#{name}="
    self.method_table[mname.to_sym] = meth
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
  
end
