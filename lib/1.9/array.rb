class Array

  class << self
    # Try to convert obj into an array, using to_ary method.
    # Returns converted array or nil if obj cannot be converted
    # for any reason. This method is to check if an argument is an array.
    def try_convert(x)
      x.to_ary if x.respond_to? :to_ary
    end
  end
  
end