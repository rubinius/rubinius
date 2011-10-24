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

  # Generic shallow copy of object.
  #
  # Copies instance variables, but does not recursively copy the
  # objects they reference. Copies taintedness.
  #
  # In contrast to .clone, .dup can be considered as creating a
  # new object of the same class and populating it with data from
  # the object.
  #
  # If class-specific behaviour is desired, the class should
  # define #initialize_copy and implement the behaviour there.
  # #initialize_copy will automatically be called on the new
  # object - the copy - with the original object as argument
  # if defined.
  #
  def dup
    copy = Rubinius.invoke_primitive(:object_class, self).allocate

    Rubinius.invoke_primitive :object_copy_object, copy, self

    Rubinius.privately do
      copy.initialize_copy self
    end
    
    copy.untrust if untrusted?
    copy
  end

  # Direct shallow copy of object.
  #
  # Copies instance variables, but does not recursively copy the
  # objects they reference. Copies taintedness and frozenness.
  #
  # In contrast to .dup, .clone can be considered to actually
  # clone the existing object, including its internal state
  # and any singleton methods.
  #
  # If class-specific behaviour is desired, the class should
  # define #initialize_copy and implement the behaviour there.
  # #initialize_copy will automatically be called on the new
  # object - the copy - with the original object as argument
  # if defined.
  #
  def clone
    # Do not implement in terms of dup. It breaks rails.
    #
    cls = Rubinius.invoke_primitive :object_class, self
    copy = cls.allocate

    Rubinius.invoke_primitive :object_copy_object, copy, self
    Rubinius.invoke_primitive :object_copy_singleton_class, copy, self

    Rubinius.privately do
      copy.initialize_copy self
    end

    copy.untrust if untrusted?
    copy.freeze if frozen?
    copy
  end
end
