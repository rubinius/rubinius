
module Ruby
  AtExit = []
end

module Kernel
  def puts(*a)
    a = [""] if a.empty?
    a.each { |obj| STDOUT.puts obj.to_s }
  end
  
  def p(*a)
    a = [nil] if a.empty?
    a.each { |obj| STDOUT.puts obj.inspect }
  end

  def print(*args)
    args.each do |obj|
      STDOUT.write obj.to_s
    end
  end
  
  def raise(exc=$!, msg=nil)
    if exc.kind_of?(String)
      exc = RuntimeError.new(exc)
    elsif msg
      cls = exc
      exc = cls.new(msg)
    end
    Ruby.asm "push exc\nraise_exc"
  end
    
  def exit(code=0)
    Process.exit(code)
  end
  
  def at_exit(&block)
    Ruby::AtExit << block
  end
end

class Process
  def self.exit(code)
    Ruby.primitive :process_exit
  end
  
  def self.micro_sleep(ms)
    Ruby.primitive :micro_sleep
  end
  
  def self.sleep(sec)
    micro_sleep(sec * 1_000_000)
  end
  
  def self.usleep(sec)
    micro_sleep(sec * 1_000)
  end
end

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
    if !@__ivars__ or @__ivars__.size == 0
      return to_s
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
    begin
      !self.class.instance_method(meth).nil?
    rescue NameError
      false
    end
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

  def instance_eval(string = nil, filename = nil, lineno = 0, &prc)
    env = prc.block.block.dup
    env.put(1, env.home.dup) # home => MethodContext
    env.home.put(9, self) # receiver => Object
    proc = Proc.from_environment(env)
    proc.call(self)
  end
  
  def instance_variables
    if !@__ivars__ or @__ivars__.size == 0
      return []
    end
    res = []
    @__ivars__.each do |k,v|
      res << k.to_s
    end
    return res
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

class GC
  def self.start
    Ruby.primitive :gc_start
  end
end

class Hash
  alias :has_key? :key?
  alias :include? :key?
  alias :member?  :key?
end
