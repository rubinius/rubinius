# TODO: Temporary! Object should find these methods in BasicObject
# but we fake that until method lookup is fixed for 1.9.
module Kernel
  def equal?(other)
    Rubinius.primitive :object_equal
    raise PrimitiveFailure, "Kernel#equal? primitive failed"
  end

  alias_method :eql?, :equal?
  alias_method :==,   :equal?
  alias_method :===,  :equal?

  def !~(other)
    self =~ other ? false : true
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
  
  def clone
    cls = Rubinius.invoke_primitive :object_class, self
    copy = cls.allocate

    Rubinius.invoke_primitive :object_copy_object, copy, self
    Rubinius.invoke_primitive :object_copy_singleton_class, copy, self

    Rubinius.privately do
      copy.initialize_copy self
    end

    copy.freeze if frozen?
    copy.untrust if untrusted?
    copy
  end
end
