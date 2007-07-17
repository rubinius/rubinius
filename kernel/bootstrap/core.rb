
class InvalidIndex < Exception
end

class Object
  
  def eql?(other)
    object_id == other.object_id
  end
  
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
    Ruby.asm <<-ASM
#local cls
push self
kind_of
    ASM
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
    return self.to_s unless @__ivars__
    
    if (@__ivars__.is_a?(Hash) or @__ivars__.is_a?(Tuple)) and @__ivars__.empty?
      return self.to_s
    end
    
    res = "#<#{self.class.name}:0x#{self.object_id.to_s(16)} "
    parts = []
    
    if @__ivars__.is_a?(Hash)    
      @__ivars__.each do |k,v|
        if v.object_id == self.object_id # This would be an infinite loop
          parts << "#{k}=self"
        else
          parts << "#{k}=#{v.inspect}"
        end
      end
    else
      0.step(@__ivars__.size - 1, 2) do |i|
        if k = @__ivars__[i]
          v = @__ivars__[i+1]
          if v.object_id == self.object_id # This would be an infinite loop
            parts << "#{k}=self"
          else
            parts << "#{k}=#{v.inspect}"
          end
        end
      end
    end
    
    res << parts.join(" ")
    res << ">"
    return res
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
  
  alias :send :__send__
  
  def method(name)
    meth = name.to_sym
    cm = Ruby.asm <<-ASM
push self
#local meth
locate_method
    ASM
    
    if cm
      return Method.new(self, cm[1], cm[0])
    else
      raise NameError, "undefined method `#{name}' for #{self.inspect}"
    end
  end
  
  def lambda
    env = Ruby.asm "push_block\n"
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

