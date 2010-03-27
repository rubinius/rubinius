module Kernel

  def equal?(other)
    Ruby.primitive :object_equal
    raise PrimitiveFailure, "Kernel#equal? primitive failed"
  end

  def eql?(other) # HACK dup of equal?
    Ruby.primitive :object_equal
    raise PrimitiveFailure, "Kernel#eql? primitive failed"
  end

  def extend(*mods)
    Rubinius.object_metaclass(self).include(*mods)
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

  # Rather than attr = !!value or attr = value && true or attr = (value
  # and true) littering code, we provide attr = value.to_bool for when
  # an attribute must be true or false.
  def to_bool
    !!self
  end

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
