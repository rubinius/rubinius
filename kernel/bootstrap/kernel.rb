module Kernel
  def equal?(other)
    Rubinius.primitive :object_equal
    raise PrimitiveFailure, "Kernel#equal? primitive failed"
  end

  alias_method :eql?, :equal?
  alias_method :==,  :equal?
  alias_method :===, :equal?

  def !~(other)
    res = self =~ other ? false : true
    Regexp.last_match = Regexp.last_match
    res
  end

  def singleton_class
    Rubinius::Type.object_singleton_class self
  end

  def extend(*mods)
    Rubinius::Type.object_singleton_class(self).include(*mods)
    self
  end

  def freeze
    Rubinius.primitive :object_freeze
    raise PrimitiveFailure, "Kernel#freeze primitive failed"
  end

  def frozen?
    Rubinius.primitive :object_frozen_p
    raise PrimitiveFailure, "Kernel#frozen? primitive failed"
  end

  def hash
    Rubinius.primitive :object_hash
    raise PrimitiveFailure, "Kernel#hash primitive failed"
  end

  def inspect
    "#<#{self.class.name}"
  end

  def respond_to_prim?(meth, visibility)
    Rubinius.primitive :object_respond_to
    meth = Rubinius::Type.coerce_to_symbol meth
    respond_to_prim?(meth, visibility)
  end

  private :respond_to_prim?

  def taint
    Rubinius.primitive :object_taint
    raise PrimitiveFailure, "Kernel#taint primitive failed"
  end

  def tainted?
    Rubinius.primitive :object_tainted_p
    raise PrimitiveFailure, "Kernel#tainted? primitive failed"
  end

  def untaint
    Rubinius.primitive :object_untaint
    raise PrimitiveFailure, "Kernel#untaint primitive failed"
  end

  def trust
    Rubinius.primitive :object_trust
    raise PrimitiveFailure, "Kernel#trust primitive failed"
  end

  def untrust
    Rubinius.primitive :object_untrust
    raise PrimitiveFailure, "Kernel#untrust primitive failed"
  end

  def untrusted?
    Rubinius.primitive :object_untrusted_p
    raise PrimitiveFailure, "Kernel#untrusted? primitive failed"
  end

  def respond_to?(meth, include_private=false)
    respond_to_prim?(meth, include_private)
  end.custom_call_site

  def respond_to_missing?(meth, include_private)
    false
  end
  private :respond_to_missing?

  def yield_gdb(obj)
    Rubinius.primitive :yield_gdb
    raise PrimitiveFailure, "Kernel#yield_gdb primitive failed"
  end
  private :yield_gdb

end
