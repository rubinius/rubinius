class InvalidIndexError < Exception
end

class Object

  # TODO move to Tuple, needed by AccessVarMethod
  def put(idx, val)
    Ruby.primitive :put
    raise InvalidIndexError, "Object#put failed."
  end

  def __find_method__(meth)
    meth = meth.to_sym
    cm = Rubinius.asm(meth) do |m|
      push :self
      run m
      push :true
      locate_method
    end

    return cm
  end

  def copy_from(other, start, dest)
    Ruby.primitive :dup_into
    raise TypeError, "unable to copy into"
  end

  def metaclass
    class << self;self;end
  end

  # TODO - Improve this check for metaclass support
  # TODO - Make this private in core
  def __verify_metaclass__
    if self.kind_of?(Fixnum) or self.kind_of?(Symbol)
      raise TypeError, "no virtual class for #{self.class}"
    end
  end

  def get_instance_variables
    Ruby.primitive :ivars_get
    raise PrimitiveFailure, "primitive failed"
  end

  def get_instance_variable(sym)
    Ruby.primitive :ivar_get
    raise PrimitiveFailure, "primitive failed"
  end

  def set_instance_variable(sym, value)
    Ruby.primitive :ivar_set
    raise PrimitiveFailure, "primitive failed"
  end

end
