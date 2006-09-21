class Type
  def initialize(str)
    @name = str.to_sym
  end
  
  def to_s
    @name.to_s
  end
  
  attr_accessor :name
  
  def eql?(b)
    @name == b.name
  end
  
  alias :== :eql?
  
  def hash
    @name.hash
  end
  
  def unknown?
    @name == :unknown
  end
  
  def become(other)
    @name = other.name
  end
  
  def inspect
    "#<Type.#{@name}>"
  end
  
  TYPES = Hash.new
  
  class << self
    def method_missing(sym)
      TYPES[sym] ||= Type.new(sym)
    end
        
    def unknown
      Type.new(:unknown)
    end
  end
  
  class Klass
    def initialize(name, sup)
      @name = name
      @superklass = sup
      @ivars = Hash.new
      @defined_methods = Hash.new
    end
    
    def super_type
      Type.method_missing(@superklass)
    end
    
    def type
      Type.method_missing(@name)
    end
    
    attr_accessor :name, :superklass, :ivars, :defined_methods
    
    def find(meth)
      @defined_methods[meth]
    end
    
    def add(func)
      @defined_methods[func.name] = func
    end
    
    alias :functions :defined_methods
  end
  
  class MapperClass
    
    def initialize
      @map = Hash.new
    end
    
    def [](type)
      @map[type]
    end
    
    def []=(mine, yours)
      @map[mine] = yours
    end
  end
  
  Mapper = MapperClass.new
end

# It's been deprecated long enough. I'm getting rid of it.
class Object
  undef :type
end

class TypedSexp < Sexp
  attr_accessor :type
  
  def set_type(t)
    
    # puts "Setting type of #{self.inspect} to #{t.inspect}"
    
    if t.kind_of? Array
      if @type.kind_of?(Array) and @type.size < t.size
        @type = t
        return self
      elsif !@type.kind_of?(Array)
         @type = t
         return self
      end
    end
    
    if t and t.unknown?
      # print "Inbound type is unknown: "
      if !@type or @type.unknown?
        # puts "using it anyway."
        @type = t
      else
        # puts "turning unknown into #{@type.inspect}"
        t.become(@type)
      end
    elsif !@type
      @type = t
    elsif @type.unknown?
      @type.become(t)
    elsif @type != t
      raise "Mis-match types #{@type.inspect} != #{t.inspect}!"
    end
    return self
  end
  
  def unify_with(idx)
    set_type self[idx].type
    return self
  end
  
  def inspect
    str = map { |e| e.inspect }.join(", ")
    if @type
      tn = @type.name
    else
      tn = "?"
    end
    "t:#{tn}(#{str})"
  end
  
  def pretty_print(q) # :nodoc:
    if @type
      tn = @type.name
    else
      tn = "?"
    end
    
    q.group(1, "t:#{tn}(", ')') do
      q.seplist(self) {|v| q.pp v }
    end
  end
  
end

def t(*a)
  TypedSexp.new(*a)
end

  
class Function
  def initialize(name, args, body, type=nil)
    @name = name
    @args = args
    @body = body
    @type = type
    @yield_args = nil
    @yield_type = nil
    @tags = nil
    @locals = Hash.new
  end
  
  attr_accessor :name, :args, :body, :type, :yield_args, :yield_type, :tags, :locals
  
  def check_args(args)
    unless @args
      @args = args.dup
      return
    end
    
    if args.size != @args.size
      raise "Mis-match argument count. #{args.inspect} <=> #{@args.inspect}"
    end
    
    args = args.dup
    
    @args.each do |a|
      t = args.shift
      if t.unknown?
        unless a.unknown?
          t.become(a)
        end
      elsif a.unknown?
        a.become(t)
      elsif a != t
        raise "Mismatch argument types. #{a} != #{t}"
      end    
    end
  end
  
  def reset_args!
    @args = nil
  end
  
  def copy_from(other)
    if @type.name == :unknown
      @type.name = other.name
    end    
  end
end

class FunctionType
  def initialize(c_func, ret_type)
    @c_func = c_func
    @ret_type = ret_type
    @arg_types = nil
  end
  
  attr_accessor :c_func, :ret_type, :arg_types
  
end

class TypeInfo
  def initialize(prefix="")
    @classes = Hash.new
    @types = Hash.new
    @prefix = prefix
    @external_types = Hash.new
    @external_functions = Hash.new { |h,k| h[k] = Hash.new }
    @consts = Hash.new
  end
  
  attr_accessor :classes, :consts, :types
  
  def add_c_type(type, str)
    @external_types[type] = str
  end
  
  def add_const(name, type)
    @consts[name] = type
  end
  
  def add_klass(name, sup)
    if obj = @classes[name]
      if obj.superklass.nil?
        obj.superklass = sup
      end
    else
      obj = Type::Klass.new(name, sup)
      @classes[name] = obj
      @types[obj.type] = obj
    end
    return obj
  end
  
  def verify_type(type)
    return if @types[type]
    
    # Create a standin class for the type for now. The idea
    # is that add_klass will get called later and fill in the
    # rest of the details.
    
    obj = Type::Klass.new(type.name, nil)
    @classes[type.name] = obj
    @types[type] = obj
    return obj
  end
  
  def find(type, function)
    if x = @external_functions[type][function]
      return x if x.inline
    end
    
    if type.unknown?
      raise "Typing incomplete. Unable to find function '#{function}' on unknown type."
    end
    
    ty = @types[type]
    if func = @types[type].find(function)
      return func
    end
    raise "Unable to find function '#{function}' for '#{type}'"
  end
  
  def find_ivar(type, name)
    klass = @types[type]
    return nil unless klass
    ivt = nil
    while !ivt and klass
      ivt = klass.ivars[name]
      return ivt if ivt
      klass = @classes[klass.superklass]
    end
    
    return nil
  end
  
  def add_function(type, function)
    return unless type
    return if type.unknown?
    to = @types[type]
    return unless to
    raise "Unknown type '#{type}'" unless to
    if cur = to.find(function.name)
      cur.copy_from function.type
    else
      @types[type].add(function)
    end
  end
  
  def to_c_func(func, type)
    "#{@prefix}#{type.name}_#{func.name}"
  end
  
  def to_c_type(type)
    "struct #{@prefix}#{type.name}"
  end
  
  def to_c_instance(type)
    if str = @external_types[type]
      return str
    end
    "#{to_c_type(type)}*"
  end
  
  def load_file(path)
    data = File.read(path)
    instance_eval(data, path, 1)
  end
  
  def cg
    @cg ||= CodeGenerator.new(self)
  end
  
  class CodeGenerator
    def initialize(info)
      @info = info
      @temps = []
      @locals = Hash.new
      @local_idx = nil
      @blocks = []
      reset_lines
    end
    
    @@local_idx = 0
    
    def self.reset
      @@local_idx = 0
    end
    
    attr_reader :info
    
    def add_local(name, type)
      @locals[name] = type
      unless @local_idx
        local_idx()
      end
    end
    
    def add_block(code)
      @blocks << code
    end
    
    def local_idx
      @local_idx ||= (@@local_idx += 1)
    end
    
    def blocks
      @blocks.join("\n")
    end
    
    def temp(type)
      t = new_temp(type)
      @temps << t
      return t
    end
    
    def new_temp(type)
      Temp.new(self, type, @temps.size)
    end      
    
    def preamble
      out = []
      
      if @local_idx
        out << "struct _locals#{@local_idx} _locals, *locals = &_locals;"
      end
      
      @temps.each do |t|
        out << "#{@info.to_c_instance(t.type)} #{t.name};"
      end
      
      out.join("\n")
    end
    
    def local_struct
      return "" unless @local_idx
      
      out = []
      @locals.each do |name, t|
        out << "#{@info.to_c_instance(t)} #{name};"  
      end
      
      "struct _locals#{@local_idx} {\n  #{out.join("\n  ")}\n};"
    end
    
    def reset_lines
      @lines = []
    end
    
    def <<(line)
      @lines << line
    end
    
    def fragment(output="")
      if @lines.empty?
        str = "#{output};"
      else
        str = @lines.join(";\n") + ";"
      end
      
      str
    end
    
    class Temp
      def initialize(cg, type, idx)
        @cg = cg
        @name = "_cg#{idx}"
        @type = type
      end
      
      attr_accessor :name, :type
      
      def to_s
        @name
      end
      
      def type_string
        @cg.info.to_c_instance(@type)
      end
      
      def cast_as(str)
        "#{@name} = (#{type_string})(#{str})"
      end
      
      def declare_as(str)
        type_string + " " + cast_as(str)
      end
    end
  end
  
  class ExternalInfo
    def initialize(type, name)
      @type = type
      @name = name
      @inline = true
      @args = []
      @gen = nil
      @block = false
      @yield_args = []
      @yield_type = Type.void
    end
    
    attr_accessor :type, :name, :inline, :args, :block, :yield_args, :yield_type
    
    def gen(&block)
      @gen = block
    end
    
    def generate(cg, recv, args, block=nil)
      if block
        unless @block
          raise "External function #{name} on #{type.inspect} does not accept a block."
        end
        
        args = args.dup
        args << block
      end
      
      out = @gen.call(cg, recv, args)
      str = cg.fragment(out)
      if @inline
        "({ #{str} })"
      else
        str
      end
    end
    
    def check_args(args)
      v_args = args
      i = 0
      if @args.size != v_args.size
        raise "Incorrect argument count (#{v_args.size} for #{@args.size})"
      end
      
      @args.each do |arg|
        v_arg = v_args[i]
        if arg.unknown? and !v_arg.unknown?
          arg.become(v_args[i])
        elsif v_arg.unknown? and !arg.unknown?
          v_arg.become(arg)
        elsif v_arg != arg
          raise "Mismatched arguments #{@args.to_a.inspect} != #{v_args.inspect}."
        end
        i += 1
      end
    end
  end
  
  def add(name, type=nil)
    type ||= @type
    info = ExternalInfo.new(type, name)
    yield info
    @external_functions[type][name] = info
  end
end
