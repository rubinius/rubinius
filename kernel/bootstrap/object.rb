class InvalidIndexError < Exception
end

class Object
  
  def at(idx)
    Ruby.primitive :at
    raise InvalidIndexError, "Object#at failed."
  end
  
  def put(idx, val)
    Ruby.primitive :put
    raise InvalidIndexError, "Object#put failed."
  end
  
  def class
    Ruby.primitive :logical_class
    raise PrimitiveFailure, "Object#class failed."
  end
  
  def object_id
    Ruby.primitive :object_id
    raise PrimitiveFailure, "Object#object_id failed."
  end
  
  def hash
    Ruby.primitive :hash_object
    raise PrimitiveFailure, "Object#hash failed."
  end

  def equal?(other)
    Ruby.primitive :object_equal
  end
  
  def kind_of?(cls)
    Rubinius.asm(cls) do |c|
      run c
      push :self
      kind_of
    end
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

  def __send__(name, *args)
    meth = name.to_sym
    count = args.size.to_i
    
    Rubinius.asm(args, meth, count) do |a,m,c|
      run a
      push_array
      push :self
      push_block
      run m
      run c
      set_args
      set_call_flags 1
      send_off_stack
    end    
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
  
  def copy_from(other, start)
    Ruby.primitive :dup_into
  end

  def dup
    Ruby.primitive :object_dup
  end
  
  def to_s
    "#<#{self.class.name}>"
  end
  
  def inspect
    "#<#{self.class.name}"
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

  def extend(*mods)
    metaclass.include(*mods)
    self
  end
  
  def method_missing(meth, *args)
    raise NoMethodError, "Unable to send #{meth}"
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

  def get_instance_variables
    Ruby.primitive :ivars_get
  end

  def get_instance_variable(sym)
    Ruby.primitive :ivar_get
  end

  def set_instance_variable(sym, value)
    Ruby.primitive :ivar_set
  end

  def taint
    Ruby.primitive :object_taint
  end
  
  def tainted?
    Ruby.primitive :object_tainted_p
  end

  def untaint
    Ruby.primitive :object_untaint
  end
  
  def freeze
    Ruby.primitive :object_freeze
  end
  
  def frozen?
    Ruby.primitive :object_frozen_p
  end
end
