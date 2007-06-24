def self.to_s
  "main"
end

class Object
  def method_missing(meth, *args)
    exc = NoMethodError.new "No method '#{meth}' on an instance of #{self.class}."
    raise exc
  end
end

class Module
  def const_missing(name)
    raise NameError, "Unable to find constant #{name}" 
  end
end
