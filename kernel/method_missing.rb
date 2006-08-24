def self.to_s
  "main"
end

class Object
  def method_missing(meth, *args)
    exc = NoMethodError.new "No method '#{meth}' on a #{self.class}."
    raise exc
  end
end