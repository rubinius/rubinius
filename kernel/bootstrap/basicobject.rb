class BasicObject
  def initialize
    # do nothing
  end
  private :initialize

  def equal?(other)
    Rubinius.primitive :object_equal
    raise ::PrimitiveFailure, "BasicObject#equal? primitive failed"
  end

  alias_method :==, :equal?

  def !
    Rubinius::Type.object_equal(self, false) ||
      Rubinius::Type.object_equal(self, nil) ? true : false
  end

  def !=(other)
    self == other ? false : true
  end

  # Send message to object with given arguments.
  #
  # Ignores visibility of method, and may therefore be used to
  # invoke protected or private methods.
  #
  # As denoted by the double-underscore, this method must not
  # be removed or redefined by user code.
  #
  def __send__(message, *args)
    Rubinius.primitive :object_send
    raise ::PrimitiveFailure, "BasicObject#__send__ primitive failed"
  end

  def __id__
    Rubinius.primitive :object_id
    raise ::PrimitiveFailure, "BasicObject#__id__ primitive failed"
  end
end
