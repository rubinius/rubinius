class Class
  def instance_fields; Ruby.asm "push self\npush 7\nfetch_field"; end
  def instance_flags; Ruby.asm "push self\npush 8\nfetch_field"; end
  ivar_as_index :superclass => 6, :instance_fields => 7, :instance_flags => 8
  
  def instance_fields=(num)
    @instance_fields = num
  end

  self.instance_fields = 9

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
  
  def superclass=(other)
    @superclass = other
  end
  
  
  def direct_superclass
    @superclass
  end
  
end
