class Module
  ivar_as_index :method_table => 1, :name => 3
    
  def method_table
    @method_table
  end
    
  def name
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
  
end
