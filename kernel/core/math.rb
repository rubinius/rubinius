module Math
  # Constants
  PI = 3.14159_26535_89793_23846
  E  = 2.71828_18284_59045_23536
  
  def sqrt(value)
    result = Platform::Math.sqrt(Float(value))
    verify_range(result, 'sqrt')
    return result
  end
  
  def verify_range(value, msg)
    Errno.handle msg
    if value.nan?
      raise Errno::EDOM, msg if Errno.const_defined?(:EDOM)
      raise Errno::ERANGE, msg if Errno.const_defined?(:ERANGE)
    end
  end
  private :verify_range

  def self.after_loaded
    module_function :sqrt
  
    private :sqrt
  end
end

