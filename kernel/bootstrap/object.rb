class InvalidIndex < Exception
end

class Object
  
  def at(idx)
    Ruby.primitive :at
    raise InvalidIndex, "Object#at failed."
  end
  
  def put(idx, val)
    Ruby.primitive :put
    raise InvalidIndex, "Object#put failed."
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
  
  def kind_of?(cls)
    Ruby.asm <<-ASM
#local cls
push self
kind_of
    ASM
  end
  
  def respond_to?(meth)
    meth = meth.to_sym
    cm = Ruby.asm <<-ASM
push self
#local meth
locate_method
    ASM
    !cm.nil?
  end

  def __send__(name, *args, &prc)
    meth = name.to_sym
    count = args.size.to_i
    Ruby.asm <<-ASM
#local args
push_array
push self
#local prc
#local meth
#local count
set_args
send_off_stack
    ASM
  end
  
  def __find_method__(meth)
    meth = meth.to_sym
    cm = Ruby.asm <<-ASM
push self
#local meth
locate_method
    ASM
    return cm
  end
  
  def copy_from(other, start)
    Ruby.primitive :dup_into
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
  
  def extend(*mods)
    metaclass.include(*mods)
  end
  
  def method_missing(meth, *args)
    raise NoMethodError, "Unable to send #{meth}"
  end

  def raise(cls, str)
    STDOUT.write "Fatal error in bootstrap/core:\n  "
    STDOUT.write str
    STDOUT.write " ("
    STDOUT.write cls.name
    STDOUT.write ")"
    STDOUT.write "\n"
    Process.exit 1 
  end
  
end