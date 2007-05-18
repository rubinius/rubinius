
require 'sexp/processor'
require 'sexp/composite_processor'
require 'translation/normalize'
require 'translation/typer'
require 'translation/to_c'

class ClassInfo
  def initialize(name)
    @name = name
    @fields = []
    @methods = Hash.new
  end
  
  def type
    Type.send(@name)
  end
  
  attr_accessor :name, :fields, :methods
end

class RsStructure
  def initialize
    @proc = CompositeSexpProcessor.new
    @proc << RsNormalizer.new
    @proc << RsOuter.new(self)
    @classes = Hash.new
    @functions = FunctionTable.new
  end
  
  attr_accessor :classes, :functions
  
  def find_class(name)
    @classes[name] ||= ClassInfo.new(name)
  end
  
  def process(x)
    @proc.process(x)
  end
end

class RsOuter < SexpProcessor
  
  def initialize(struct)
    super()
    self.strict = true
    self.auto_shift_type = true
    @structure = struct
  end
  
  def process_class(x)
    name = x.shift
    sup = x.shift
    body = x.shift
    
    unless Symbol === name[1]
      raise "Unsupported class name '#{name.inspect}'"
    end
    
    name = name[1]
    
    clsinfo = @structure.find_class(name)
    
    prc = RsClassBody.create(clsinfo, @structure)
    prc.process body
    
  end
end

class RsClassBody < SexpProcessor
  
  def self.create(*a)
    prc = CompositeSexpProcessor.new
    prc << RsNormalizer.new
    prc << RsClassBody.new(*a)
    return prc
  end

  def initialize(clsinfo, struct)
    super()
    self.strict = true
    self.auto_shift_type = true
    self.require_expected = false
    @structure = struct
    @info = clsinfo
    @fields = []
  end
  
  def process_scope(x)
    proces x.shift
  end
  
  def process_block(x)
    until x.empty?
      process x.shift
    end
  end
  
  def process_call(x)
    recv = x.shift
    name = x.shift
    if recv == [:self] and name == :attr_accessor
      # Currently, just look at all the ivars used in the methods
      # to figure out the set of fields.
      x.clear
      return
      
      args = x.shift
      args.shift
      args.each do |elem|
        if elem.first == :lit
          @fields << elem.last.to_sym
        else
          raise "Unsupported type in attr_accessor. #{elem.inspect}"
        end
      end
    else
      raise "Unsupported meta-method '#{name}'"
    end
  end
  
  def method_name(meth)
    "rs_#{@info.name}_#{meth}"
  end
  
  def process_defn(x)
    name = x.shift
    args = x.shift
    top = x.shift
    body = top[1]
    
    func = @structure.functions
    
    rpc = RsToC.new(func, @info.fields)
    string = rpc.process body
    @info.methods[name] = string
    ft = FunctionType.new method_name(name), rpc.return_type
    func.add_function @info.type, name, ft
    return nil
  end
end