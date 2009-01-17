module Kernel

  def clone
    Ruby.primitive :object_clone
    raise TypeError, "unable to clone"
  end

  def dup
    Ruby.primitive :object_dup
    raise TypeError, "unable to dup"
  end

  def equal?(other)
    Ruby.primitive :object_equal
    raise PrimitiveFailure, "Object#equal? failed."
  end

  def eql?(other) # HACK dup of equal?
    Ruby.primitive :object_equal
    raise PrimitiveFailure, "Object#eql? failed."
  end

  def extend(*mods)
    metaclass.include(*mods)
    self
  end

  def freeze
    Ruby.primitive :object_freeze
    raise PrimitiveFailure, "primitive failed"
  end

  def frozen?
    Ruby.primitive :object_frozen_p
    raise PrimitiveFailure, "primitive failed"
  end

  def hash
    Ruby.primitive :object_hash
    raise PrimitiveFailure, "Object#hash failed."
  end

  def inspect
    "#<#{self.class.name}"
  end

  def object_id
    Ruby.primitive :object_id
    raise PrimitiveFailure, "Object#object_id failed."
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

  def taint
    Ruby.primitive :object_taint
    raise PrimitiveFailure, "primitive failed"
  end

  def tainted?
    Ruby.primitive :object_tainted_p
    raise PrimitiveFailure, "primitive failed"
  end

  def untaint
    Ruby.primitive :object_untaint
    raise PrimitiveFailure, "primitive failed"
  end

  def yield_gdb(obj)
    Ruby.primitive :yield_gdb
    raise PrimitiveFailure, "primitive failed"
  end

end
