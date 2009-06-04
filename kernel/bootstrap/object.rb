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

  def get_instance_variable(sym)
    Ruby.primitive :object_get_ivar
    raise PrimitiveFailure, "Object#get_instance_variable primitive failed"
  end
  private :get_instance_variable

  def set_instance_variable(sym, value)
    Ruby.primitive :object_set_ivar
    raise PrimitiveFailure, "Object#set_instance_variable primitive failed"
  end
  private :set_instance_variable

  def __fixnum__
    Ruby.primitive :object_is_fixnum
    kind_of? Fixnum
  end
end
