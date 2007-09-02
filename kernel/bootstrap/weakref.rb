class WeakRef
  
  class RefError < RuntimeError; end
  
  def self.create_weakref(obj)
    Ruby.primitive :make_weak_ref
  end
  
  def initialize(object)
    @wr = create_weakref(object)
  end
  
  def weakref_alive?
    !@wr.at(0).nil?
  end
  
  def object
    obj = @wr.at(0)
    raise RefError, "Object has been collected as garbage" unless obj
    return obj
  end
  
end
