require 'compiler/system_hints'

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

    if flags
      binding = flags[:binding]
    else
      binding = nil
    end

    comp = new(Compiler::Generator, binding)
    node = comp.convert_sexp([:eval_expression, sexp])
    cm = node.to_description(:__eval_script__).to_cmethod
    cm.file = filename.to_sym
    return cm
  end

  def initialize(gen_class, binding=nil)
    @variables = {}
    @generator_class = gen_class
    @call_plugins = []

    @file = "(unknown)"
    @line = 0
    @binding = binding

    load_plugins
  end
  
  def custom_scopes?
    @binding
  end
  
  def create_scopes
    ctx = @binding.context
    if ctx.kind_of? BlockContext
      all_scopes = []
      block_scopes = []
      
      while ctx.kind_of? BlockContext
        scope = Compiler::LocalScope.new(nil)
        scope.from_eval = true
        block_scopes.unshift scope
        all_scopes << scope
        
        if !ctx.env.from_eval? and names = ctx.method.local_names
          i = 0
          names.each do |name|
            scope[name].created_in_block! i
            i += 1
          end
        end
        
        ctx = ctx.env.home_block
      end
      
      scope = Compiler::LocalScope.new(nil)
      scope.from_eval = true
      all_scopes << scope
      
      if names = ctx.method.local_names
        i = 0
        names.each do |name|
          scope[name].slot = i
          i += 1
        end
      end
      
      return [scope, block_scopes, all_scopes, @binding.context]
    else
      scope = Compiler::LocalScope.new(nil)
      scope.from_eval = true
      i = 0
      if names = ctx.method.local_names
        names.each do |name|
          scope[name].slot = i
          i += 1
        end
      end
      
      return [scope, [], [scope], @binding.context]
    end
  end

  attr_reader :call_plugins
  attr_accessor :generator_class

  def set_position(file, line)
    @file, @line = file, line
  end

  def current_file
    @file
  end

  def current_line
    @line
  end

  def load_plugins
    # The default plugins
    activate :block_given
    activate :primitive
    activate :assembly
    activate :method_visibility
    activate :fastmath
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
require 'compiler/bytecode'
require 'compiler/generator'
require 'compiler/plugins'

