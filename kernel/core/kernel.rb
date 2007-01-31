module Kernel
  
  def Float(o)
    return o.to_f if o.respond_to?(:to_f)
    raise TypeError, "can't convert #{o.class} into Float"
  end
  
  def Integer(o)
    return o.to_i if o.respond_to?(:to_i)
    raise TypeError, "can't convert #{o.class} into Integer"
  end

end