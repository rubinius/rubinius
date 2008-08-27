class InvalidIndexError < Exception
end

class Object

  def __find_method__(meth)
    meth = meth.to_sym
    tup = Rubinius.asm(meth) do |m|
      push :self
      run m
      push :true
      locate_method
    end

    return tup
  end

  def __show__
    Ruby.primitive :object_show
  end

  def metaclass
    class << self;self;end
  end
  alias_method :__metaclass__, :metaclass  
  # TODO - Improve this check for metaclass support
  # TODO - Make this private in core
  def __verify_metaclass__
    if self.__kind_of__(Fixnum) or self.__kind_of__(Symbol)
      raise TypeError, "no virtual class for #{self.class}"
    end
  end

  def get_instance_variables
    Ruby.primitive :object_get_ivars
    raise PrimitiveFailure, "primitive failed"
  end

  def get_instance_variable(sym)
    Ruby.primitive :object_get_ivar
    raise PrimitiveFailure, "primitive failed"
  end

  def set_instance_variable(sym, value)
    Ruby.primitive :object_set_ivar
    raise PrimitiveFailure, "primitive failed"
  end

  def become!(obj)
    Ruby.primitive :object_become
    raise PrimitiveFailure, "primitive failed"
  end
end
