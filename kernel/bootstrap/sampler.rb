class Sampler
  
  def activate(hz)
    Ruby.primitive :sampler_activate
  end
  
  def terminate
    Ruby.primitive :sampler_stop
  end
  
end