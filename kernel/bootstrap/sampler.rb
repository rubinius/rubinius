class Sampler
  
  def activate(hz)
    Ruby.primitive :sampler_activate
    raise PrimitiveFailure, "primitive failed"
  end
  
  def terminate
    Ruby.primitive :sampler_stop
    raise PrimitiveFailure, "primitive failed"
  end
  
end
