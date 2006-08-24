
module Kernel
  def puts(obj)
    STDOUT.puts obj.to_s
  end
  
  def p(obj)
    STDOUT.puts obj.inspect
  end
  
  def print(obj)
    STDOUT.write obj.to_s
  end
  
  def raise(exc, msg=nil)
    if msg
      cls = exc
      exc = cls.new(msg)
    end
    Ruby.asm "push exc\nraise_exc"
  end
  
end

class InvalidIndex < Exception
end

class Object
  def initialize
    put 0, {}
  end
  
  def at(idx)
    Ruby.primitive :at
    exc = InvalidIndex.new("Could not access index #{idx} of #{self}")
    raise exc
  end
  
  def put(idx, val)
    Ruby.primitive :put
    exc = InvalidIndex.new("Could not write to index #{idx} of #{self}")
    raise exc
  end
  
  def fields
    Ruby.primitive :fields
  end
    
  def class
    Ruby.primitive :logical_class
  end
  
  def kind_of?(cls)
    self.class < cls
  end
  
  def object_id
    Ruby.primitive :object_id
  end
  
  def hash
    Ruby.primitive :hash_object
  end
  
  def copy_from(other)
    Ruby.primitive :dup_into
  end
  
  def dup
    nw = self.class.allocate
    nw.copy_from(self)
    return nw
  end
  
  def ==(other)
    object_id == other.object_id
  end
  
  def to_s
    "#<#{self.class.name}:0x#{self.object_id.to_s(16)}>"
  end
  
  def inspect
    to_s
  end
  
  def nil?
    false
  end
  
  def undef?
    false
  end
end

class TrueClass
  def to_s
    "true"
  end
end

class FalseClass
  def to_s
    "false"
  end
end