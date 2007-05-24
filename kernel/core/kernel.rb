module Functions
  module Foreign
    attach_function nil, 'srand', [:uint], :void
    attach_function nil, 'rand', [], :int
  end

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
  
  def warn(warning)
    unless $VERBOSE.nil?
      $stderr.write warning
      $stderr.write "\n"
    end
    nil
  end

  def exit(code=0)
    raise SystemExit.new(code)
  end

  def exit!(code=0)
    Process.exit(code)
  end
  
  # NOTE - this isn't quite MRI compatible, we don't store return the previous
  # seed value from srand and we don't seed the RNG by default with a combination
  # of time, pid and sequence number
  def srand(seed)
    Foreign.srand(seed.to_i)
  end

  def rand(max=nil)
    x = Foreign.rand
    # scale result of rand to a domain between 0 and max
    if max.nil? or max.zero?
      x / 0x7fffffff.to_f
    else
      max = max.abs
      if max < 0x7fffffff
        x / (0x7fffffff / max)
      else
         x * (max / 0x7fffffff)
      end
    end
  end
  
  alias fail raise
end

class SystemExit < Exception
  def code
    at(0)
  end

  def message
    "System is exiting with code '#{code}'"
  end
end
