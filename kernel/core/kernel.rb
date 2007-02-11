module Kernel
  def Float(obj)
    obj = obj.to_f
    if Float === obj
      obj
    else
      raise TypeError, "#{obj.class}#to_f should return Float"
    end
  rescue NoMethodError
    Integer(obj).to_f
  rescue TypeError
    raise TypeError, "can't convert #{obj.class} into Float"
  end
  
  def Integer(obj)
    obj = obj.to_i
    if Integer === obj
      obj
    else
      raise TypeError, "#{obj.class}#to_i should return Integer"
    end
  rescue NoMethodError
    raise TypeError, "can't convert #{obj.class} into Integer"
  end
end

