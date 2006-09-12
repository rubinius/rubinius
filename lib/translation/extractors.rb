require 'translation/typer'
require 'translation/normalize'

class ExtractClasses < SexpProcessor
  def initialize
    super
    @classes = Hash.new
    @depth = []
    self.auto_shift_type = true
    self.expected = Array
  end
  
  attr_accessor :classes
  
  def process_class(x)
    name = x.shift.last
    sup = x.shift
    if sup
      sup = sup.last
    else
      sup = :Object
    end
    obj = Type::Klass.new(name, sup)
    @classes[name] = obj
    @depth << obj
    out = process x.shift
    @depth.pop
    out
  end
  
  include RsNormalizer::DefnNormalize
  
  def process_defn(x)
    x = super
    x.shift
    name = x.shift
    args = x.shift
    
    meth = Method.new(name, args, nil)
    
    meth.body = process x.shift
    @depth.last.defined_methods[name] = meth
    []
  end
  
  def process_iasgn(x)
    name = x.shift.to_s[1..-1].to_sym
    val = process x.shift
    @depth.last.ivars[name] = true
    val
  end
end

class ExtractRequires < SexpProcessor
  
  def self.from_file(path)
    pr = ExtractRequires.new
    io = File.open(path)
    syd = SydneyParser.load_file io
    
  end
  
  def initialize
    super
    self.auto_shift_type = true
    self.expected = Array
    @requires = []
  end
  
  attr_accessor :requires
  
  def process_call(x)
    who = x.shift
    name = x.shift
    args = x.shift
    
    if who == [:self] and name == :require
      args.shift
      @requires += args.map { |s| s.last }
    end
  end
end