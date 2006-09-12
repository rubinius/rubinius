class Class
  def alias_method(nw,cur)
    meth = methods[cur]
    unless meth
      exc = NoMethodError.new("No method by the name of '#{cur}' for #{self}")
      raise exc
    end
    
    methods[nw] = meth
    return meth
  end
  
  def instance_fields=(num)
    put 5, num
  end
  
end