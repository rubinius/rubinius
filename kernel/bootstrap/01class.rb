class Class
  
  def instance_fields=(num)
    @instance_fields = num
  end
    
  def class_variable_set(name, val)
    @variables ||= Hash.new
    @variables[name] = val
  end
  
  def class_variable_get(name)
    @variables ||= Hash.new
    @variables[name]
  end
  
end
