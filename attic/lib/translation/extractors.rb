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

class ExtractHints < SexpProcessor
  def initialize(coms=[], check_lines=false)
    super()
    self.expected = Array
    @check_lines = check_lines
    @hints = Hash.new
    @current_class = nil
    @comments = Hash.new
    coms.each do |c|
      @comments[c.first] = c.last
    end
  end
  
  attr_reader :hints
  
  def process(x)
    unless @check_lines
      return super
    end
    
    unless x.first == :block
      if Fixnum === x.last
        @line_no = x.pop
      end
    end
    p :___________
    p x
    out = super(x)
    p out
    return out
  end
  
  def process_class(x)
    x.shift
    name = x.shift
    sup = x.shift
    body = x.shift
    
    @current_class = name.last
    begin
      nb = process(body)
    ensure
      @current_class = nil
    end
    
    return [:class, name, sup, nb]
  end
  
  def parse_comment(str)
    str.strip!
    hint = []
    if m = /T:\s*([\w\s]*) => (\w*)/.match(str)
      ret = $2
      args = $1.split(/\s*,\s*/).map { |a| Type.method_missing(a.strip) }
      hint = [:hint, :type, ::Type.method_missing(ret), args]
    elsif m = /hints:\s*([\w\s]*)/.match(str)
      tags = $1.split(/\s*,\s*/)
      hint = [:hint, :tag, tags]
    end
        
    return hint    
  end
  
  def process_comment(x)
    x.shift
    parse_comment x.shift
  end
  
  def process_block(x)
    x.shift
    @hint = nil
    while node = x.shift
      cur = process(node)
      if cur.first == :hint
        @hint = cur
      else
        @hint = nil
      end
    end
    return []
  end
  
  def process_defn(x)
    x.shift
    name = x.shift
    if @hint
      @hints[[@current_class, name]] = @hint
    end
    
    if hint = @comments[@line_no - 1]
      @hints[[@current_class, name]] = parse_comment(hint)
    end
    
    x.clear
    []
  end
  
  def process_defs(x)
    x.shift
    recv = x.shift
    return unless recv == [:self]
    name = x.shift
    @hints[[[@current_class, :self], name]] = @hint
    x.clear
    []
  end
end
