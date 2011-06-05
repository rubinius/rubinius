class BasicObject
  def equal?(other)
    Ruby.primitive :object_equal
    raise PrimitiveFailure, "BasicObject#equal? primitive failed"
  end

  alias_method :eql?, :equal?
  alias_method :==,   :equal?

  def !
    equal?(false) || equal?(nil) ? true : false
  end

  def !=(other)
    self == other ? false : true
  end
end
