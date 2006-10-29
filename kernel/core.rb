
module Ruby
  AtExit = []
end

module Kernel
  def puts(obj="")
    STDOUT.puts obj.to_s
  end
  
  def p(obj)
    STDOUT.puts obj.inspect
  end
  
  def print(*args)
    args.each do |obj|
      STDOUT.write obj.to_s
    end
  end
  
  def raise(exc, msg=nil)
    if msg
      cls = exc
      exc = cls.new(msg)
    end
    Ruby.asm "push exc\nraise_exc"
  end
  
  def load(path)
    cm = CompiledMethod.load_from_file(path)
    cm.activate_as_script
  end
  
  def require(thing)
    $:.each do |dir|
      path = "#{dir}/#{thing}.rbc"
      if File.exists?(path)
        $" << path
        return load(path)
      end
    end
    raise LoadError, "Unable to find '#{thing}' to load"
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
  
  def to_s
    "#<#{self.class.name}:0x#{self.object_id.to_s(16)}>"
  end
  
  def inspect
    to_s
  end
  
  def find_method(meth)
    cur = self.class
    while cur
      cm = cur.methods[meth]
      return [cm, cur] if cm
      cur = cur.direct_superclass
    end
    return nil
  end
  
  def respond_to?(meth)
    not find_method(meth).nil?
  end
  
  def __send__(meth, *args)
    cm, mod = find_method(meth)
    if cm.nil?
      raise NameError.new("Unable to locate method '#{meth}' on #{self}")
    end
    
    cm.activate(self, args)
  end
  
  alias :send :__send__
  
  def method(name)
    cm, mod = find_method(name)
    if cm.nil?
      raise NameError.new("Unable to locate method '#{name}' on #{self}")      
    end
    
    mo = Method.new(self, mod, cm)
    return mo
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
