class BasicObject
  ##
  # :call-seq:
  #   obj.instance_eval(string [, filename [, lineno]] )   => obj
  #   obj.instance_eval { | | block }                      => obj
  #
  # Evaluates a string containing Ruby source code, or the given block, within
  # the context of the receiver +obj+. In order to set the context, the
  # variable +self+ is set to +obj+ while the code is executing, giving the
  # code access to +obj+'s instance variables. In the version of
  # #instance_eval that takes a +String+, the optional second and third
  # parameters supply a filename and starting line number that are used when
  # reporting compilation errors.
  #
  #   class Klass
  #     def initialize
  #       @secret = 99
  #     end
  #   end
  #   k = Klass.new
  #   k.instance_eval { @secret }   #=> 99

  def instance_eval(string=nil, filename="(eval)", line=1, &prc)
    if ::ImmediateValue === self
      sc = nil
    else
      sc = ::Rubinius::Type.object_singleton_class(self)
    end

    if prc
      if string
        raise ::ArgumentError, 'cannot pass both a block and a string to evaluate'
      end
      # Return a copy of the BlockEnvironment with the receiver set to self
      env = prc.block

      if sc
        constant_scope = env.repoint_scope sc
      else
        constant_scope = env.disable_scope!
      end

      return env.call_under(self, constant_scope, true, self)
    elsif string
      string = ::Kernel.StringValue(string)

      constant_scope = ::Rubinius::ConstantScope.of_sender

      if sc
        constant_scope = ::Rubinius::ConstantScope.new(sc, constant_scope)
      else
        constant_scope = constant_scope.using_disabled_scope
      end

      binding = ::Binding.setup(::Rubinius::VariableScope.of_sender,
                                ::Rubinius::CompiledCode.of_sender,
                                constant_scope)

      c = ::Rubinius::ToolSets::Runtime::Compiler
      be = c.construct_block string, binding, filename, line

      be.call_on_instance(self)
    else
      raise ::ArgumentError, 'block not supplied'
    end
  end

  ##
  # :call-seq:
  #   obj.instance_exec(arg, ...) { |var,...| block }  => obj
  #
  # Executes the given block within the context of the receiver +obj+. In
  # order to set the context, the variable +self+ is set to +obj+ while the
  # code is executing, giving the code access to +obj+'s instance variables.
  #
  # Arguments are passed as block parameters.
  #
  #   class Klass
  #     def initialize
  #       @secret = 99
  #     end
  #   end
  #
  #   k = Klass.new
  #   k.instance_exec(5) { |x| @secret+x }   #=> 104

  def instance_exec(*args, &prc)
    raise ::LocalJumpError, "Missing block" unless block_given?
    env = prc.block

    return prc.ruby_method.call(*args) if prc.ruby_method

    constant_scope = env.constant_scope
    if ::ImmediateValue === self
      constant_scope = constant_scope.using_disabled_scope
    else
      sc = ::Rubinius::Type.object_singleton_class(self)
      constant_scope = constant_scope.using_current_as(sc)
    end

    return env.call_under(self, constant_scope, true, *args)
  end
end

module Kernel

  # Names of local variables at point of call (including evaled)
  #
  def local_variables
    locals = []

    scope = Rubinius::VariableScope.of_sender

    # Ascend up through all applicable blocks to get all vars.
    while scope
      if scope.method.local_names
        scope.method.local_names.each do |name|
          locals << name
        end
      end

      if dyn = scope.dynamic_locals
        dyn.keys.each do |name|
          locals << name unless locals.include?(name)
        end
      end

      scope = scope.parent
    end

    locals
  end
  module_function :local_variables

  # Obtain binding here for future evaluation/execution context.
  #
  def binding
    return Binding.setup(
      Rubinius::VariableScope.of_sender,
      Rubinius::CompiledCode.of_sender,
      Rubinius::ConstantScope.of_sender,
      Rubinius::VariableScope.of_sender.self,
      Rubinius::Location.of_closest_ruby_method
    )
  end
  module_function :binding

  # Evaluate and execute code given in the String.
  #
  def eval(string, binding=nil, filename=nil, lineno=nil)
    string = StringValue(string)
    filename = StringValue(filename) if filename
    lineno = Rubinius::Type.coerce_to lineno, Fixnum, :to_i if lineno
    lineno = 1 if filename && !lineno

    if binding
      binding = Rubinius::Type.coerce_to_binding binding
      filename ||= binding.constant_scope.active_path
    else
      binding = Binding.setup(Rubinius::VariableScope.of_sender,
                              Rubinius::CompiledCode.of_sender,
                              Rubinius::ConstantScope.of_sender,
                              self)

      filename ||= "(eval)"
    end

    lineno ||= binding.line_number

    existing_scope = binding.constant_scope
    binding.constant_scope = existing_scope.dup

    c = Rubinius::ToolSets::Runtime::Compiler
    be = c.construct_block string, binding, filename, lineno

    result = be.call_on_instance(binding.self)
    binding.constant_scope = existing_scope
    result
  end
  module_function :eval
  private :eval
end

class Module

  #--
  # These have to be aliases, not methods that call instance eval, because we
  # need to pull in the binding of the person that calls them, not the
  # intermediate binding.
  #++

  def module_eval(string=undefined, filename="(eval)", line=1, &prc)
    # we have a custom version with the prc, rather than using instance_exec
    # so that we can setup the ConstantScope properly.
    if prc
      unless undefined.equal?(string)
        raise ArgumentError, "cannot pass both string and proc"
      end

      # Return a copy of the BlockEnvironment with the receiver set to self
      env = prc.block
      constant_scope = env.repoint_scope self
      return env.call_under(self, constant_scope, true, self)
    elsif undefined.equal?(string)
      raise ArgumentError, 'block not supplied'
    end

    string = StringValue(string)
    filename = StringValue(filename)

    # The constantscope of a module_eval CM is the receiver of module_eval
    cs = Rubinius::ConstantScope.new self, Rubinius::ConstantScope.of_sender

    binding = Binding.setup(Rubinius::VariableScope.of_sender,
                            Rubinius::CompiledCode.of_sender,
                            cs)

    c = Rubinius::ToolSets::Runtime::Compiler
    be = c.construct_block string, binding, filename, line

    be.call_under self, cs, true, self
  end

  alias_method :class_eval, :module_eval
end
