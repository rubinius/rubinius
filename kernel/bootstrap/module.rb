class Module
  ivar_as_index :method_table => 1, :name => 3, :constants => 4, :parent => 5, :superclass => 6
    
  def method_table
    @method_table
  end

  def constant_table
    @constants
  end

  def parent
    @parent
  end
    
  def name
    unless @name
      Object.constants_table.each do |klass_name, klass|
        if klass.equal? self
          @name = klass_name
        end
      end
    end
    @name.to_s
  end
  
  # Ultra simple private
  def private(name)
    if cm = @method_table[name]
      if cm.kind_of? Tuple
        cm.put 0, :private
      else
        tup = Tuple.new(2)
        tup.put 0, :private
        tup.put 1, cm
        @method_table[name] = tup
      end
    end
  end
  
  def __find_method(namesym)
    Ruby.primitive :find_method
  end
  
  def alias_method(new_name, current_name)
    meth = @method_table[current_name]
    unless meth
      raise NoMethodError, "Unable to find method '#{current_name}' to alias to '#{new_name}'"
    end
    @method_table[new_name] = meth
    VM.reset_method_cache(new_name)
  end
  
  # 'superclass' method defined in class.rb, 
  # because it is more complex than a mere accessor
  def superclass=(other)
    @superclass = other
  end
  
  def direct_superclass
    @superclass
  end

  def append_features(mod)
    im = IncludedModule.new(self)
    im.attach_to mod
  end
  
  def included(mod); end
  
  def include(mod)    
    mod.append_features(self)
    mod.included(self)
    self
  end
  
  def attr_reader(name)
    sym = "@#{name}".__symbol_lookup__
    meth = AccessVarMethod.get_ivar(sym)
    @method_table[name] = meth
    return nil
  end
  
  def attr_writer(name)
    sym = "@#{name}".__symbol_lookup__
    meth = AccessVarMethod.get_ivar(sym)
    @method_table[name] = meth
    return nil    
  end
  
  def attr_accessor(name)
    attr_reader(name)
    attr_writer(name)
    return true
  end
  
end
