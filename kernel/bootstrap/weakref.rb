class WeakRef

  class RefError < RuntimeError; end

  def self.new
    Ruby.primitive :weakref_new
    raise PrimitiveFailure, "WeakRef.new failed"
  end

  def __setobj__(obj)
    Ruby.primitive :weakref_set_object
    raise PrimitiveFailure, "WeakRef#__setobj__ failed"
  end

  def __object__
    Ruby.primitive :weakref_object
    raise PrimitiveFailure, "WeakRef#object failed"
  end

  def object
    obj = __object__()
    raise RefError, "Object has been collected as garbage" unless obj
    return obj
  end

  alias_method :__getobj__, :object
  alias_method :object=, :__setobj__

  def inspect
    "#<WeakRef:0x#{object_id.to_s(16)} object=#{object.inspect}>"
  end

  def weakref_alive?
    !!__object__
  end

end
