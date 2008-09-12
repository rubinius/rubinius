
##
# Turns text into CompiledMethods

class Compiler

  Config = Hash.new

  ##
  # Compiler error subclass.

  class Error < RuntimeError
  end

  def self.process_flags(flags)
    flags.each { |f| Config[f] = true } if flags
  end

  def self.parse_flags(stream)
    to_clear = []
    stream.each do |token|
      if token.prefix? "-f"
        to_clear << token
        name, val = token[2..-1].split("=")
        val = true unless val
        Config[name] = val
      end
    end

    to_clear.each { |t| stream.delete(t) }
  end

  def self.compile_file(path, flags=nil)
    process_flags(flags)
    sexp = File.to_sexp(path, true)

    comp = new(Generator)
    node = comp.into_script(sexp)
    return node.to_description(:__script__).to_cmethod
  end

  def self.compile_string(string, flags = {}, file = "(eval)", line = 1)
    sexp = string.to_sexp(file, line, true)

    binding = flags[:binding]
    node    = new(Generator, binding).convert_sexp([:eval_expression, sexp])
    cm      = node.to_description(:__eval_script__).to_cmethod
    cm.file = file.to_sym

    return cm
  end

  TimeEpoch = 1141027200 # rubinius' birthday

  @version_number = nil

  def self.version_number
    unless @version_number
      begin
        # handled for .rba files in init.rb
        dir = $LOAD_PATH.detect { |path| File.file? "#{path}/compiler/compiler.rb" }
        max = Dir["#{dir}/compiler/*.rb"].map { |f| File.mtime(f).to_i }.max
        @version_number = max - TimeEpoch
      rescue Exception
        @version_number = 0
      end

      if $DEBUG_LOADING
        STDERR.puts "[Compiler version: #{@version_number}]"
      end
    end
    return @version_number
  end

  def self.version_number=(ver)
    if ver
      @version_number = ver - TimeEpoch
    else
      @version_number = 0
    end

    if $DEBUG_LOADING
      STDERR.puts "[Compiler version: #{@version_number} (forced)]"
    end
  end

  def initialize(gen_class, binding=nil)
    @variables = {}
    @generator_class = gen_class
    @plugins = Hash.new { |h,k| h[k]= [] }

    @file = "(unknown)"
    @line = 0
    @binding = binding

    @kernel = Config['rbx-kernel']
    load_plugins
  end

  def kernel?
    @kernel
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
        scope = LocalScope.new(nil)
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

      scope = LocalScope.new(nil)
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
      scope = LocalScope.new(nil)
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

  attr_reader :plugins
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
    activate_default :block_given
    activate_default :primitive
    activate_default :assembly
    activate_default :fastmath
    activate_default :current_method
    activate :safemath if Config['rbx-safe-math']
    activate :const_epxr if Config['rbx-kernel']
    activate_default :fastsystem
    activate_default :fastgeneric
    # AutoPrimitiveDetection is currently disabled
    # TODO - Implement the opt_* primitives it requires and reactivate
    # activate_default :auto_primitive
  end

  def activate_default(name)
    activate(name) unless Config["no-#{name}"]
  end

  def activate(name)
    cls = Plugins.find_plugin(name)
    raise Error, "Unknown plugin '#{name}'" unless cls
    @plugins[cls.kind] << cls.new(self)
  end

  def inspect
    "#<#{self.class}>"
  end

  def convert_sexp(sexp)
    return nil if sexp.nil?
    raise ArgumentError, "input must be a Sexp: #{sexp.inspect}" unless
      Sexp === sexp

    klass = Node::Mapping[sexp.first]

    raise Error, "Unable to resolve '#{sexp.first.inspect}'" unless klass

    return klass.create(self, sexp)
  end

  def into_script(sexp)
    begin
      convert_sexp([:script, sexp])
    rescue Object => e
      puts "Compilation error detected: #{e.message}"
      puts "   near #{@file}:#{@line}"
      puts
      puts e.awesome_backtrace.show
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

  ##
  # Raised when turning the AST into bytecode fails in some way.

  class GenerationError < Error; end

  def show_errors(gen) # TODO: remove
    yield
  end

end

require 'compiler/nodes'
require 'compiler/local'
require 'compiler/bytecode'
require 'compiler/generator'
require 'compiler/plugins'

