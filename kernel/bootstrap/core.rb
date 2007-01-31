
class InvalidIndex < Exception
end

class Object
  include Kernel
  
  def initialize
  end
      
  def class
    Ruby.primitive :logical_class
  end
  
  def object_id
    Ruby.primitive :object_id
  end
  
  def hash
    Ruby.primitive :hash_object
  end
  
  def nil?
    false
  end
  
  def undef?
    false
  end
  
  def kind_of?(cls)
    self.class < cls
  end
  
  def instance_of?(cls)
    self.class == cls
  end
  
  alias :is_a? :kind_of?
  
  def copy_from(other, start)
    Ruby.primitive :dup_into
  end
  
  def dup
    nw = self.class.allocate
    nw.copy_from(self, 0)
    return nw
  end
  
  def equal?(other)
    object_id == other.object_id
  end
  
  alias :==   :equal?
  alias :eql? :equal?
  alias :===  :equal?

  def to_s
    "#<#{self.class.name}:0x#{self.object_id.to_s(16)}>"
  end
  
  def inspect
    if !@__ivars__ || !@__ivars__.is_a?(Array) || @__ivars__.empty?
      return self.to_s
    end
    
    res = "#<#{self.class.name}:0x#{self.object_id.to_s(16)} "
    parts = []
    @__ivars__.each do |k,v|
      parts << "#{k}=#{v.inspect}"
    end
    res << parts.join(" ")
    res << ">"
    return res
  end
  
  def respond_to?(meth)
    meth = meth.to_sym
    cur, cm = self.class.__find_method(meth)
    !cm.nil?
  end
  
  def __send__(name, *args, &prc)
    meth = self.class.instance_method(name)
    meth = meth.bind(self)
    meth.call(*args, &prc)
  end
  
  alias :send :__send__
  
  def method(name)
    self.class.instance_method(name).bind(self)
  end
  
  def lambda
    env = nil
    Ruby.asm "push_block\nset env\n"
    unless env
      raise ArgumentError, "Unable to create a Proc if a block is not passed in"
    end
    
    return Proc.from_environment(env)
  end

end

TRUE = true
FALSE = false

class TrueClass
  def &(other)
    (other.nil? or other == false) ? false : true
  end
  
  def ^(other)
    (other.nil? or other == false) ? true : false
  end
  
  def |(other)
    true
  end
  
  def to_s
    "true"
  end
  
  alias :inspect :to_s
end

class FalseClass
  def &(other)
    false
  end
  
  def ^(other)
    (other == false or other.nil?) ? false : true
  end
  
  alias :| :^
  
  def to_s
    "false"
  end
  
  alias :inspect :to_s
end
