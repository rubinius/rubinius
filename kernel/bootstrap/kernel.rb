module Kernel
  # Send message to object with given arguments.
  #
  # Ignores visibility of method, and may therefore be used to
  # invoke protected or private methods.
  #
  # As denoted by the double-underscore, this method must not
  # be removed or redefined by user code.
  #
  def __send__(message, *args)
    Ruby.primitive :object_send

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

  def equal?(other)
    Ruby.primitive :object_equal
    raise PrimitiveFailure, "Kernel#equal? primitive failed"
  end

  def eql?(other) # HACK dup of equal?
    Ruby.primitive :object_equal
    raise PrimitiveFailure, "Kernel#eql? primitive failed"
  end

  alias_method :==,  :equal?
  alias_method :===, :equal?

  def singleton_class
    Rubinius::Type.object_singleton_class self
  end

  def extend(*mods)
    Rubinius::Type.object_singleton_class(self).include(*mods)
    self
  end

  def freeze
    Ruby.primitive :object_freeze
    raise PrimitiveFailure, "Kernel#freeze primitive failed"
  end

  def frozen?
    Ruby.primitive :object_frozen_p
    raise PrimitiveFailure, "Kernel#frozen? primitive failed"
  end

  def hash
    Ruby.primitive :object_hash
    raise PrimitiveFailure, "Kernel#hash primitive failed"
  end

  def inspect
    "#<#{self.class.name}"
  end

  def __id__
    Ruby.primitive :object_id
    raise PrimitiveFailure, "Kernel#object_id primitive failed"
  end

  def respond_to?(meth, include_private=false)
    Ruby.primitive :object_respond_to_public
    respond_to_all?(meth.to_sym, include_private);
  end

  def respond_to_all?(meth, include)
    Ruby.primitive :object_respond_to
    raise PrimitiveFailure, "Kernel#prim_respond_to? failed"
  end

  private :respond_to_all?

  def taint
    Ruby.primitive :object_taint
    raise PrimitiveFailure, "Kernel#taint primitive failed"
  end

  def tainted?
    Ruby.primitive :object_tainted_p
    raise PrimitiveFailure, "Kernel#tainted? primitive failed"
  end

  def untaint
    Ruby.primitive :object_untaint
    raise PrimitiveFailure, "Kernel#untaint primitive failed"
  end

  def yield_gdb(obj)
    Ruby.primitive :yield_gdb
    raise PrimitiveFailure, "Kernel#yield_gdb primitive failed"
  end
  private :yield_gdb

end
