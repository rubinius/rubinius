class Class
    
  # Need to get rid of this cleverness
  def new(*args)
    obj = allocate()
    block = Ruby.asm "push_block"
    if block
      obj.initialize(*args, &block)
    else
      obj.initialize(*args)
    end
    obj
  end

  def self.new(sclass=Object)
    raise TypeError, "superclass must be a Class (#{sclass.class.name} given)" unless Class === sclass
    
    obj = Rubinius.class_constitute(sclass, nil)
    block = Ruby.asm "push_block"
    obj.class_eval(&block) if block
    # add clas to sclass's subclass list, for ObjectSpace.each_object(Class)
    # NOTE: This is non-standard; Ruby does not normally track subclasses
    sclass.inherited(obj) if sclass.respond_to?(:inherited)
    obj
  end

  def opened_class_cv(cls)
    cls = Object unless cls
    cls.add_subclass(self)
    # FIXME: We shouldn't have to do this; hook calls should preserve the stack
    self
  end

  # NOTE: The next two methods are not standard Ruby; JRuby implements them, but not public
  def add_subclass_cv(cls)
    @subclasses ||= []
    @subclasses << cls
  end

  def subclasses_cv(descend = false)
    if descend
      subclasses_descend()
    else
      @subclasses.dup
    end
  end

  def subclasses_descend(all = [])
    return unless @subclasses
    @subclasses.each {|cls| all << cls; cls.subclasses_descend(all)}
    all
  end
  
  def class_variable_set(name, val)
    raise NameError, "#{name} is not an allowed class variable name" unless name.to_s[0..1] == '@@'

    @variables ||= Hash.new
    @variables[name.to_sym] = val
  end
  
  def class_variable_get(name)
    raise NameError, "#{name} is not an allowed class variable name" unless name.to_s[0..1] == '@@'

    @variables ||= Hash.new
    @variables[name.to_sym]
  end

  def class_variables(symbols = false)
    @variables ||= Hash.new
    if symbols
      @variables.keys
    else
      @variables.keys.map {|k| k.to_s}
    end
  end
  
  def ancestors
    out = [self]
    sup = direct_superclass()
    while sup
      if sup.class == IncludedModule
        out << sup.module
      else
        out << sup
      end
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
      
      mod = mod.direct_superclass
    end
    nil
  end
  
  def alias_method_cv(new_name, current_name)
    meth = find_method_in_hierarchy(current_name)
    if meth
      method_table[new_name] = meth
      VM.reset_method_cache(new_name)
    else
      if self.kind_of? MetaClass        
        raise NameError, "Unable to find '#{current_name}' for object #{self.attached_instance.inspect}"
      else
        raise NameError, "undefined method `#{current_name}' for class `#{self.name}'"
      end
    end
  end
  
  def attr_reader_cv(*args)
    args.each do |name|
      sym = "@#{name}".to_sym
      meth = AccessVarMethod.get_ivar(sym)
      self.method_table[name.to_sym] = meth
    end
    return nil
  end
  
  def attr_writer_cv(*args)
    args.each do |name|
      sym = "@#{name}".to_sym
      meth = AccessVarMethod.set_ivar(sym)
      mname = "#{name}=".to_sym
      self.method_table[mname] = meth
    end
    return nil
  end
  
  def attr_accessor_cv(*names)
    names.each do |name|
      attr_reader(name)
      attr_writer(name)
    end
    return true
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
  
  def inspect
    if name
      name
    else
      super.inspect
    end
  end
  
  def self.after_loaded
    alias_method :attr_reader, :attr_reader_cv
    alias_method :attr_writer, :attr_writer_cv
    alias_method :attr_accessor, :attr_accessor_cv
    alias_method :alias_method, :alias_method_cv
    alias_method :opened_class, :opened_class_cv
    alias_method :add_subclass, :add_subclass_cv
    alias_method :subclasses, :subclasses_cv
  end

end
