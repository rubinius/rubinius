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
class BasicObject
  def method_missing(meth, *args)
    ::Kernel.raise ::NoMethodError, "Unable to send '#{meth}' on instance of BasicObject"
  end
  private :method_missing

  def singleton_method_added(name) end
  private :singleton_method_added

  def singleton_method_removed(name) end
  private :singleton_method_removed

  def singleton_method_undefined(name) end
  private :singleton_method_undefined

  def __all_instance_variables__
    Rubinius.primitive :object_ivar_names

    raise PrimitiveFailure, "BasicObject#__instance_variables__ failed"
  end
  private :__all_instance_variables__

  def __instance_variables__
    ary = []
    __all_instance_variables__.each do |sym|
      ary << sym if sym.is_ivar?
    end

    ary
  end
end
