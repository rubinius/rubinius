class Object
  def metaclass
    class << self;self;end
  end
  alias_method :__metaclass__, :metaclass

  def get_instance_variables
    Ruby.primitive :object_get_ivars
    raise PrimitiveFailure, "Object#get_instance_variables primitive failed"
  end
  private :get_instance_variables

  def __fixnum__
    Ruby.primitive :object_is_fixnum
    kind_of? Fixnum
  end
end
