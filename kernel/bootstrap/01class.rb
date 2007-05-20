class Class
  def superclass; Ruby.asm "push self\npush 6\nfetch_field"; end
  def instance_fields; Ruby.asm "push self\npush 7\nfetch_field"; end
  def instance_flags; Ruby.asm "push self\npush 8\nfetch_field"; end
  ivar_as_index :superclass => 6, :instance_fields => 7, :instance_flags => 8
  
  def instance_fields=(num)
    @instance_fields = num
  end

  self.instance_fields = 9

  def class_variable_set(name, val)
    raise ArgumentError, "#{name} is not an allowed class variable name" unless name.to_s[0..1] == '@@'

    @variables ||= Hash.new
    @variables[name.to_sym] = val
  end
  
  def class_variable_get(name)
    raise ArgumentError, "#{name} is not an allowed class variable name" unless name.to_s[0..1] == '@@'

    @variables ||= Hash.new
    @variables[name.to_sym]
  end
  
  def superclass=(other)
    @superclass = other
  end
end
