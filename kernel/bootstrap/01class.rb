class Class
  
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
  
end
