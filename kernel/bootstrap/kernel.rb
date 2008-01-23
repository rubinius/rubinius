module Kernel

  def __send__(*args)
    Ruby.primitive :object_send
    raise RuntimeError, "send failed"
  end

  def class
    Ruby.primitive :logical_class
    raise PrimitiveFailure, "Object#class failed."
  end

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
    Ruby.primitive :hash_object
    raise PrimitiveFailure, "Object#hash failed."
  end

  def inspect
    "#<#{self.class.name}"
  end

  # Returns true if the given Class is either the class or superclass of the
  # object or, when given a Module, if the Module has been included in
  # object's class or one of its superclasses. Returns false otherwise. If the
  # argument is not a Class or Module, a TypeError is raised.
  def kind_of?(cls)
    unless cls.class.equal?(Class) or cls.class.equal?(Module)
      raise TypeError, 'kind_of? requires a Class or Module argument'
    end

    Rubinius.asm(cls) do |c|
      run c
      push :self
      kind_of
    end
  end

  def method_missing(meth, *args)
    raise NoMethodError, "Unable to send #{meth}"
  end

  def object_id
    Ruby.primitive :object_id
    raise PrimitiveFailure, "Object#object_id failed."
  end

  def raise(cls, str, junk=nil)
    STDOUT.write "Fatal error in bootstrap/core:\n  "
    STDOUT.write str
    STDOUT.write " ("
    STDOUT.write cls.name
    STDOUT.write ")"
    STDOUT.write "\n"
    Process.exit 1
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

  def to_s
    "#<#{self.class.name}>"
  end

  def untaint
    Ruby.primitive :object_untaint
    raise PrimitiveFailure, "primitive failed"
  end

  def yield_gdb(obj)
    Ruby.primitive :yield_gdb
  end

end

