class Compiler
  
  class Error < RuntimeError
  end
  
  def self.compile_file(path, flags=nil)
    sexp = File.to_sexp(path, true)
    
    comp = new(Compiler::Generator)
    node = comp.into_script(sexp)
    return node.to_description(:__script__).to_cmethod
  end
    
  def self.compile_string(string, flags=nil, filename="(eval)", line=1)
    sexp = string.to_sexp(filename, line, true)
    
    comp = new(Compiler::Generator)
    node = comp.into_script(sexp)
    return node.to_description(:__eval_script__).to_cmethod
  end
  
  def initialize(gen_class)
    @variables = {}
    @generator_class = gen_class
    @call_plugins = []
    
    @file = "(unknown)"
    @line = 0
    
    load_plugins
  end
  
  attr_reader :call_plugins
  attr_accessor :generator_class
  
  def set_position(file, line)
    @file, @line = file, line
  end
  
  def load_plugins    
    # The default plugins
    activate :block_given
    activate :primitive
    activate :assembly
  end
  
  def activate(name)
    cls = Compiler::Plugins.find_plugin(name)
    raise Error, "Unknown plugin '#{name}'" unless cls
    @call_plugins << cls.new(self)
  end
  
  def inspect
    "#<#{self.class}>"
  end
    
  def convert_sexp(sexp)
    return nil if sexp.nil?
    
    klass = Compiler::Node::Mapping[sexp.first]
    
    raise Error, "Unable to resolve #{sexp.first}" unless klass

    return klass.create(self, sexp)
  end
  
  def into_script(sexp)
    begin
      convert_sexp([:script, sexp])
    rescue Object => e
      puts "Compilation error detected: #{e.message}"
      puts "   near #{@file}:#{@line}"
      puts
      puts e.backtrace.show
    end
  end
  
  def get(tag)
    @variables[tag]
  end
  
  def set(tag, val=true)
    if tag.kind_of? Hash
      cur = @variables.dup
      @variables.merge! tag
      begin
        yield
      ensure
        @variables = cur
      end
    else
      cur = @variables[tag]
      @variables[tag] = val
      begin
        yield
      ensure
        @variables[tag] = cur
      end
    end
  end
end

require 'compiler/nodes'
require 'compiler/local'
require 'compiler/generator'
require 'compiler/plugins'

