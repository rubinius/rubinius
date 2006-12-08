class Class
  def alias_method(nw,cur)
    meth = @methods[cur]
    unless meth
      raise NoMethodError, "No method by the name of '#{cur}' for #{self}"
    end
    
    @methods[nw] = meth
    return meth
  end
  
  def instance_fields=(num)
    @instance_fields = num
  end
  
end
