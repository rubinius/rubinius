class DelegatedMethod < RuntimePrimitive
  self.instance_fields = 7
  
  def self.build(sym, receiver, pass_self)
    obj = allocate()
    obj.put 1, RuntimePrimitive::DispatchAsMethod
    obj.put 2, 0
    obj.put 3, 0 # serial number
    obj.put 4, sym
    obj.put 5, receiver
    obj.put 6, pass_self # If true, first argument will be 'self'
    return obj
  end
end
