require 'rubygems'
require 'sexp/simple_processor'
require 'cuby'
require 'sydparse'

class CubyPrimitives
  def initialize(header_path=".")
    @headers = header_path
    @funcs = Cuby::HeaderParser.find_in_directory(@headers).values.flatten
    @output = {}
    detect_mappings
  end
  
  attr_accessor :output
  
  def detect_mappings
    maps = Hash.new { |h,k| h[k] = {}}
    @funcs.each do |func|
      m = /(.*?)_(.*)/.match(func.name)
      next unless m
      next if m[1].empty?
      map = m[1].downcase.to_sym
      name = m[2]
      arg = func.arguments.first
      if arg and arg[:type] == "STATE"
        map_to = "#{func.name}(state, %s)"
      else
        map_to = name
      end
      
      maps[map][name.to_sym] = map_to
    end
    
    @maps = maps
  end
    
  def new_cuby
    c = Cuby.new
    %w!_int j k m!.each do |var|
      c.declare_var "int", var
    end
    
    %w!self _lit t1 t2 t3!.each do |var|
      c.declare_var "OBJECT", var
    end
    
    c.declare_var "rstate", "state"
    
    c.map_operator = :as
    c.declare_method = :declare
    
    add_header_maps(c)
    add_builtin_methods(c)
    
    c.declare_function 'int', 'sizeof', ['void']
    
    c.add_method(:fail) do
      "_ret = FALSE; break"
    end
    
    c.on_return = proc { |val|
      "stack_push(#{val}); break"
    }
    
    c.true_value = "Qtrue"
    c.false_value = "Qfalse"
    return c
  end
  
  def declare_function(cuby, func)
    args = func.arguments.map { |a| a[:type] }
    cuby.declare_function func.return_type, func.name, args
  end
  
  ObjectMap = {
    :fixnum? => "FIXNUM_P(%s)",
    :to_int => "FIXNUM_TO_INT(%s)"
  }
  
  NumericMap = {
    :to_fix => "I2N(%s)"
  }
  
  CharStarMap = {
    :set_value => ["char", proc { |s, v|
      "*#{s} = #{v}"
    }],
    
    :[] => ["char", proc { |s,a|
      "#{s}[#{a}]"
    }]
  }
  
  StringMap = {
    :as_char => ["char", proc { |s| "'#{s[1..-2]}'"}]
  }
  
  Operators = [:+, :==, :-, :*, :<, :>, :-@, :%, :/]
    
  def add_header_maps(cuby)
    @funcs.each do |func|
      declare_function(cuby, func)
    end
    
    @maps.each do |name, map|
      cuby.add_map name, map
    end
    
    cuby.add_map :object, ObjectMap
    cuby.add_map :numeric, NumericMap
    cuby.add_map :charstar, CharStarMap
    cuby.add_map :string, StringMap
    cuby.add_type_map "OBJECT", :object
    cuby.add_type_map "int", :numeric
    cuby.add_type_map "char*", :charstar
    
    Operators.each do |op|
      cuby.operators << op
    end
  end
  
  def add_builtin_methods(c)
    
  end
  
  def fixup_body(args, c_code)
    preamble = "self = stack_pop();\n"
    args.each do |a|
      preamble << "#{a} = stack_pop();\n"
    end
    
    return preamble + c_code
  end
  
  class Processor < SimpleSexpProcessor
    def initialize(cont, output)
      super()
      self.require_expected = false
      self.strict = true
      self.auto_shift_type = true
      
      @cont = cont
      @output = output
    end
    
    def process_class(x)
      x.shift
      x.shift
      process x.shift
    end
    
    def process_scope(x)
      process x.shift
    end
    
    def process_block(x)
      while e = x.shift
        process e
      end
    end
    
    def process_defn(x)
      name = x.shift
      body = x.shift
      
      an = body[1].delete_at(1)
      args = an[1]
            
      c = @cont.new_cuby
      c.generate_from body
      
      @output[name] = @cont.fixup_body(args, c.code)
    end
  end
  
  def parse_file(path)
    io = File.open(path)
    syd = SydneyParser.load_file io
    io.close
    
    pro = Processor.new(self, @output)
    pro.process syd.sexp
  end
end

cp = CubyPrimitives.new("shotgun/lib")
cp.parse_file ARGV.shift
cp.output.each do |meth, code|
  p meth
  puts code
end