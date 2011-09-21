class BasicObject
  def equal?(other)
    Rubinius.primitive :object_equal
    raise PrimitiveFailure, "BasicObject#equal? primitive failed"
  end

  alias_method :==,   :equal?

  def !
    equal?(false) || equal?(nil) ? true : false
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

    # MRI checks for Fixnum explicitly and raises ArgumentError
    # instead of TypeError. Seems silly, so we don't bother.
    #
    case message
    when String
      message = Rubinius::Type.coerce_to message, Symbol, :to_sym
    when Symbol
      # nothing!
    else
      raise TypeError, "#{message.inspect} is not a symbol"
    end

    __send__ message, *args
  end
end
