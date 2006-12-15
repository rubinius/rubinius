require 'sexp/simple_processor'

class Cuby
  
  class Error < RuntimeError
  end
  
  class UnknownMethod < Error
  end
  
  class ExternalFunction
    def initialize(name, ret, args)
      @name = name
      @type = ret
      @arguments = args
    end
    
    attr_reader :type
  end
  
  class Variable
    def initialize(cuby, name, type)
      @cuby = cuby
      @type = type.strip
      @name = name
      @pointer = false
      parse_type
      parse_name
      @maps = []
    end
    
    attr_reader :name, :type
    
    def struct?
      !@struct_name.nil?
    end
    
    def pointer?
      @pointer
    end
    
    attr_reader :struct_name
    
    def parse_type
      if m = /struct ([^\*]+)(\*?)/.match(@type)
        @struct_name = m[1].to_sym
        @pointer = (m[2] == "*")
        @type = "struct #{@struct_name}"
      elsif m = /(.*)(\*?)/.match(@type)
        @type = m[1]
        @pointer = (m[2] == "*")
      end
    end
    
    def parse_name
      if @name.to_s[0] == ?*
        @name = @name.to_s[1..-1].to_sym
        @pointer = true
      end
    end
    
    def add_map(map)
      @maps << map
    end
    
    def find_mapping(name)
      fm = @maps.find { |m| m.key?(name) }
      return fm if fm
      tm = @cuby.find_type_mapping(@type)
      return nil unless tm
      return tm if tm.key?(name)
      return nil
    end
  end
  
  class MacroMethod
    def initialize(name, code)
      @name = name
      @code = code
    end
    
    def apply(*a)
      @code.call(*a)
    end
  end
  
  class Struct
    def initialize(name)
      @name = name
      @fields = Hash.new
      @order = []
    end
    
    def add_field(type, name)
      @fields[name] = type
      @order << name
    end
    
    def field?(name)
      @fields.key?(name)
    end
    
    def type_of(fel)
      @fields[fel]
    end
  end
  
  def initialize
    @functions = Hash.new
    @variables = Hash.new
    @methods = Hash.new
    @structs = Hash.new
    @code = ""
    @true_value = "1"
    @false_value = "0"
    @operators = []
    @method_maps = Hash.new
    @map_operator = nil
    @type_map = Hash.new
    @last_type = nil
    @literal_types = Hash.new
    @declare_method = nil
    @on_return = nil
  end
  
  def reset
    @code = ""
    @last_type = nil
  end
  
  def operator?(name)
    @operators.include?(name)
  end
  
  attr_reader :code, :functions, :variables, :operators, :methods
  attr_reader :structs, :method_maps, :literal_types
  attr_accessor :true_value, :false_value, :map_operator
  attr_accessor :last_type, :declare_method, :on_return
  
  def declare_function(ret, name, args)
    name = name.to_sym
    @functions[name] = ExternalFunction.new(name, ret, args)
  end
  
  def declare_var(type, name)
    name = name.to_sym
    var = Variable.new(self, name, type)
    @variables[var.name] = var
    return var
  end
  
  def add_method(meth, &block)
    @methods[meth] = MacroMethod.new(meth, block)
  end
  
  def add_struct(name)
    str = Struct.new(name)
    @structs[name] = str
    yield str
  end
  
  def resolve_struct(name)
    var = @variables[name]
    if var and var.struct?
      return @structs[var.struct_name]
    end
    
    return nil
  end
  
  def add_map(name, map)
    if cur = @method_maps[name]
      cur.update map
    else
      @method_maps[name] = map
    end
  end
  
  def add_type_map(type, map_name)
    @type_map[type] = map_name
  end
  
  def last_type_map
    @method_maps[@type_map[@last_type]]
  end
  
  def find_type_mapping(type)
    if mn = @type_map[type]
      return @method_maps[mn]
    end
    return nil
  end
  
  def calculate_literal_map(obj)
    case obj
    when Numeric
      :numeric
    when String
      :string
    else
      nil
    end
  end
  
  def set_literal_type(klass, type)
    @literal_types[klass] = type
  end
  
  class Processor < SimpleSexpProcessor
    def initialize(cuby)
      super()
      @cuby = cuby
      self.auto_shift_type = true
      self.expected = String
    end
    
    def process(x)
      # Reset the last type each time through.
      @cuby.last_type = nil
      super
    end
    
    def process_fcall(x)
      name = x.shift
      args = x.shift
      
      func = @cuby.functions[name]
      raise Error, "Unknown function #{name}" unless func
      @cuby.last_type = func.type
      
      return "#{name}()" unless args
      
      args = args[1..-1].map { |a| process(a) }
      if args.empty?
        "#{name}()"
      else
        "#{name}(#{args.join(', ')})"
      end
    end
    
    def process_vcall(x)
      name = x.shift
      if macro = @cuby.methods[name]
        x.clear
        return macro.apply()
      end
      
      func = @cuby.functions[name]
      unless func
        var = @cuby.variables[name]
        if var
          @cuby.last_type = var.type
          return "#{name}"
        end
        raise Error, "Unknown function #{name}" unless func
      end
      @cuby.last_type = func.type
      "#{name}()"
    end
    
    def process_lasgn(x)
      name = x.shift
      var = @cuby.variables[name]
      raise Error, "Unknown variable '#{name}'" unless var
      @cuby.last_type = nil
      idx = x.shift
      val = process(x.shift)
      
      "#{name} = #{val}"
    end
    
    def process_lvar(x)
      name = x.shift
      var = @cuby.variables[name]
      raise Error, "Unknown variable '#{name}'" unless var
      idx = x.shift
      @cuby.last_type = var.type
      "#{name}"
    end
    
    def process_self(x)
      process_lvar([:self, 0])
    end
    
    def process_block(x)
      lines = []
      while e = x.shift
        out = process(e)
        lines << out unless out.empty?
      end
            
      return lines.join(";\n") + ";\n"
    end
    
    def process_scope(x)
      process x.shift
    end
    
    def process_lit(x)
      val = x.shift
      @cuby.last_type = @cuby.literal_types[val.class]
      if Fixnum === val
        return val.to_s
      else
        raise Error, "Unknown literal #{x.inspect}"
      end
    end
    
    def process_or(x)
      lhs = process(x.shift)
      rhs = process(x.shift)
      
      "(#{lhs} || #{rhs})"
    end
    
    def process_and(x)
      lhs = process(x.shift)
      rhs = process(x.shift)
      
      "(#{lhs} && #{rhs})"
    end
    
    def process_not(x)
      code = process(x.shift)
      "!#{code}"
    end
    
    def process_true(x)
      @cuby.true_value.dup
    end
    
    def process_false(x)
      @cuby.false_value.dup
    end
    
    def process_break(x)
      "break"
    end
    
    def process_next(x)
      "continue"
    end
    
    def process_str(x)
      x.shift.dump
    end
    
    def process_call(x)
      rex = x.shift
      name = x.shift
      
      if rex.first == :lvar
        lvar_accessed = rex[1]
        vi = @cuby.variables[lvar_accessed]
      elsif rex.first == :self
        lvar_accessed = :self
        vi = @cuby.variables[:self]
      else
        lvar_accessed = nil
      end
      
      if rex.first == :lit or rex.first == :str
        lit_accessed = rex[1]
      else
        lit_accessed = nil
      end
      
      if @cuby.declare_method == name and rex.first == :vcall
        data = x.shift
        if data.size == 3
          kind = data[1].last
          dec = data.last.last
        else
          kind = data.last.last
        end
        
        if Symbol === kind
          kind = @cuby.literal_types[Object.const_get(kind)]
        end
        @cuby.declare_var kind, rex.last
        
        if dec
          return dec
        else
          return ""
        end
      end
      
      recv = process(rex)
      recv_map = @cuby.last_type_map
      
      if @cuby.operator?(name)
        arg = x.shift
        if arg
          rhs = process(arg[1])
          return "#{recv} #{name} #{rhs}"
        else
          return "#{name}#{recv}"
        end
      elsif macro = @cuby.methods[name]
        x.clear
        return macro.apply(recv)
      elsif str = @cuby.resolve_struct(lvar_accessed)
        r_s = name.to_s
        if r_s[-1] == ?=
          fel = r_s[0..-2].to_sym
          if str.field?(fel)
            val = process(x.shift[1])
            @cuby.last_type = str.type_of(fel)
            if vi.pointer?
              return "#{recv}->#{fel} = #{val}"
            else
              return "#{recv}.#{fel} = #{val}"
            end
          end
        elsif str.field?(r_s.to_sym)
          x.clear
          @cuby.last_type = str.type_of(r_s.to_sym)
          if vi.pointer?
            return "#{recv}->#{r_s}"
          else
            return "#{recv}.#{r_s}"
          end
        end
        
        raise Error, "Unknown field '#{fel}' for #{str.inspect}"
      elsif @cuby.map_operator == name
        map_name = x.shift[1].last
        unless lvar_accessed
          raise Error, "Unable to apply mapping to anything but a local variable"
        end
        
        unless vi
          raise Error, "Unknown local variable '#{vi}' to apply map to"
        end
        
        vi.add_map @cuby.method_maps[map_name.to_sym]
        return ""        
      elsif vi and map = vi.find_mapping(name)
        func = map[name]
        return call_function(recv, func, x)
      elsif lit_accessed or recv_map
        if lit_accessed
          map_name = @cuby.calculate_literal_map(lit_accessed)
          map = @cuby.method_maps[map_name]
        elsif recv_map
          map = recv_map
        end
        
        if map and op = map[name]
          return call_function(recv, op, x)
        end
      end
      
      raise UnknownMethod, "Unsupported call '#{name}' applied to '#{recv}'"
    end
    
    def call_function(recv,op,x)
      na = x.shift
      if na
        args = na[1..-1].map { |a| process(a) }
      else
        args = []
      end
      
      if Array === op
        @cuby.last_type = op[0]
        call = op[1]
      else
        name = op
        if ft = @cuby.functions[name.to_sym]
          @cuby.last_type = ft.type
        else
          @cuby.last_type = nil
        end
      end
      
      
      if call
        return call.call(recv, *args)
      elsif op.index("%s")
        args.unshift recv
        expanded = op % [args.join(", ")]
        return expanded
      elsif args.empty?
        return "#{op}(#{recv})"
      else
        return "#{op}(#{recv}, #{args.join(", ")})"
      end
    end
    
    def process_if(x)
      cond = x.shift
      thn = x.shift
      els = x.shift
      
      cond_str = process(cond)
      
      unless thn
        return "if(#{cond_str}) {}" unless els
        cond_str = "!(#{cond_str})"
        thn = els
        els = nil
      end
      
      str =  "if(#{cond_str}) {\n"
      
      no_block = (thn.first != :block)
      str << process(thn)
      str << ";\n" if no_block
      
      if els
        str << "} else {\n"

        no_block = (els.first != :block)
        str << process(els)
        str << ";\n" if no_block
      end
      
      str << "}"
      
      return str
    end
    
    def detect_block(x)
      [x, x.first != :block]
    end
    
    def process_while(x, untl=false)
      cond = x.shift
      body, no_block = detect_block x.shift
      prefixed = x.shift
      
      cond_str = process(cond)
      cond_str = "!(#{cond_str})" if untl
      
      if prefixed
        str = "while(#{cond_str}) {\n"
        str << process(body)
        str << ";\n" if no_block
        str << "}"
      else
        str = "do {\n"
        str << process(body)
        str << ";\n" if no_block
        str << "} while(#{cond_str});"
      end
      
      return str
    end
    
    def process_until(x)
      process_while(x, true)
    end
    
    def process_return(x)
      val = x.shift
      val = process(val) if val
      if @cuby.on_return
        return @cuby.on_return.call(val)
      end
      
      if val
        "return #{val}"
      else
        "return"
      end
    end
  end
  
  def generate_from(code)
    pro = Processor.new(self)
    str = pro.process code
    @code << str
  end
  
  class FunctionDeclaration
    def initialize
      @name = nil
      @return_type = nil
      @arguments = []
    end
    
    attr_accessor :name, :return_type, :arguments
  end
  
  class HeaderParser
    
    def self.find_in_file(path)
      parse = new()
      out = []
      File.readlines(path).each do |line|
        func = parse.parse_declaration(line)
        out << func if func
      end
      
      return out
    end
    
    def self.find_in_directory(dir, re=/.h$/)
      require 'find'
      out = {}
      Find.find(dir) do |path|
        if File.file?(path) and re.match(path)
          funcs = find_in_file(path)
          out[path] = funcs unless funcs.empty?
        end
      end
      
      return out
    end
    
    def initialize
    end
    
    PartsRegex = /(.*)\s+([^\s]+)\(([^\)]*)\)\s*;/
    
    def seperate_parts(dec)
      m = PartsRegex.match(dec)
      return nil unless m
      ret = {}
      ret[:return_type] = m[1]
      ret[:name] = m[2]
      ret[:arguments] = m[3].split(/\s*,\s*/)
      if ret[:name][0] == ?*
        ret[:name] = ret[:name][1..-1]
        ret[:return_type] << "*"
      end
      return ret
    end
    
    def parse_argument(arg)
      parts = arg.split(/\s+/)
      ret = {}
      pointer = false
      # get rid of the const
      parts.shift if parts.first == "const"
      
      if parts.first == "struct"
        parts.shift
        type = "struct #{parts.first}"
      else
        type = parts.first
      end
      ret[:type] = type
      if type[-1] == ?*
        pointer = true
        ret[:pointed_type] = type[0..-2]
      end
      
      if parts.size == 1
        ret[:name] = nil
      else
        name = parts.last
        if name[0] == ?*
          ret[:pointed_type] = ret[:type].dup
          ret[:type] << "*"
          name = name[1..-1]
          pointer = true
        end
        ret[:name] = name
      end
      ret[:pointer] = pointer
      return ret
    end
    
    def parse_declaration(dec)
      parts = seperate_parts(dec)
      return nil unless parts
      
      out = FunctionDeclaration.new
      out.name = parts[:name]
      out.return_type = parts[:return_type]
      out.arguments = parts[:arguments].map do |a|
        parse_argument(a)
      end
      return out
    end
  end
  
end
