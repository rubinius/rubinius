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

  def Array(obj)
    ret = obj.respond_to?(:to_ary) ? obj.to_ary : obj.to_a
    if Array === ret
      ret
    else
      raise TypeError, "#{obj.class}#to_a did not return Array"
    end
  rescue NoMethodError
    raise TypeError, "can't convert #{obj.class} into Array"
  end

  def String(obj)
    obj.to_s
  end
  

  def exit(code=0)
    raise SystemExit.new(code)
  end

  def exit!(code=0)
    Process.exit(code)
  end
  
end

class SystemExit < Exception
  def code
    at(0)
  end

  def message
    "System is exiting with code '#{code}'"
  end
end
