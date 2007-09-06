module Math
  # Constants
  PI = 3.14159_26535_89793_23846
  E  = 2.71828_18284_59045_23536
  
  def sqrt(value)
    Platform::Math.sqrt(Float(value))
  end

  def self.after_loaded
    module_function :sqrt
  
    private :sqrt
  end
end

