
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
  
  def load(path)
    cm = CompiledMethod.load_from_file(path)
    raise LoadError, "Unable to load file at path: #{path}" unless cm
    cm.activate_as_script
  end
  
  def require(thing)
    filename = thing + ".rbc"
    if File.exists?(filename)
      $" << filename
      return load(filename)
    else
      $:.each do |dir|
        path = "#{dir}/#{filename}"
        if File.exists?(path)
          $" << path
          return load(path)
        end
      end
      raise LoadError, "Unable to find '#{thing}' to load"
    end
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
    to_s
  end
  
  def respond_to?(meth)
    begin
      !self.class.instance_method(meth).nil?
    rescue NameError
      false
    end
  end
  
  def __send__(meth, *args, &prc)
    meth = self.class.instance_method(meth)
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

class GC
  def self.start
    Ruby.primitive :gc_start
  end
end
