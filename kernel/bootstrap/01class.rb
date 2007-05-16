class Class
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def methods; Ruby.asm "push self\npush 1\nfetch_field"; end
  def method_cache; Ruby.asm "push self\npush 2\nfetch_field"; end
  def name; Ruby.asm "push self\npush 3\nfetch_field"; end
  def constants; Ruby.asm "push self\npush 4\nfetch_field"; end
  def parent; Ruby.asm "push self\npush 5\nfetch_field"; end
  def superclass; Ruby.asm "push self\npush 6\nfetch_field"; end
  def instance_fields; Ruby.asm "push self\npush 7\nfetch_field"; end
  def instance_flags; Ruby.asm "push self\npush 8\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :methods => 1, :method_cache => 2, :name => 3, :constants => 4, :parent => 5, :superclass => 6, :instance_fields => 7, :instance_flags => 8
  
  def instance_fields=(num)
    @instance_fields = num
  end
    
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
