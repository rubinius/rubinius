module Kernel

  def clone
    Ruby.primitive :object_clone
    raise TypeError, "Kernel#clone primitive failed"
  end

  def dup
    Ruby.primitive :object_dup
    raise TypeError, "Kernel#dup primitive failed"
  end

  def equal?(other)
    Ruby.primitive :object_equal
    raise PrimitiveFailure, "Kernel#equal? primitive failed"
  end

  def eql?(other) # HACK dup of equal?
    Ruby.primitive :object_equal
    raise PrimitiveFailure, "Kernel#eql? primitive failed"
  end

  def extend(*mods)
    metaclass.include(*mods)
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

  def object_id
    Ruby.primitive :object_id
    raise PrimitiveFailure, "Kernel#object_id primitive failed"
  end

  def respond_to?(meth,include_private=false)
    meth = meth.to_sym
    cm = Rubinius.asm(meth, include_private) do |m,i|
      push :self
      run m
      run i
      locate_method
    end
    !cm.nil?
  end

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

end
