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
          name = name.to_s
          locals << name
        end
      end

      # Should not have any special cases here
      if dyn = scope.dynamic_locals
        dyn.keys.each {|name| locals << name.to_s }
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
      Rubinius::CompiledMethod.of_sender,
      Rubinius::StaticScope.of_sender)
  end
  module_function :binding

  # Evaluate and execute code given in the String.
  #
  def eval(string, binding=nil, filename=nil, lineno=1)
    if binding
      if binding.kind_of? Proc
        binding = binding.binding
      elsif binding.respond_to? :to_binding
        binding = binding.to_binding
      end

      unless binding.kind_of? Binding
        raise ArgumentError, "unknown type of binding"
      end

      # shortcut for checking for a local in a binding!
      # This speeds rails up quite a bit because it uses this in rendering
      # a view A LOT.
      #
      # Rails always does this passing in a binding, so thats why the check
      # is here.
      #
      # TODO eval the AST rather than compiling. Thats slightly slower than
      # this, but handles infinitely more cases.
=begin
      if m = /^\s*defined\? ([a-z_][A-Za-z0-9_]*)\s*$/.match(string)
        local = m[1].to_sym
        if binding.variables.local_defined?(local)
          return "local-variable"
        else
          return nil
        end
      end
=end

      filename ||= binding.static_scope.active_path
    else
      binding = Binding.setup(Rubinius::VariableScope.of_sender,
                              Rubinius::CompiledMethod.of_sender,
                              Rubinius::StaticScope.of_sender)

      filename ||= "(eval)"
    end

    cm = Rubinius::Compiler.compile_eval string, binding.variables, filename, lineno
    cm.scope = binding.static_scope.dup
    cm.name = :__eval__

    yield cm if block_given?

    # This has to be setup so __FILE__ works in eval.
    script = Rubinius::CompiledMethod::Script.new(filename, true)
    script.eval_binding = binding

    cm.scope.script = script

    # Internalize it now, since we're going to springboard to it as a block.
    cm.compile

    be = Rubinius::BlockEnvironment.new
    be.under_context binding.variables, cm

    # Pass the BlockEnvironment this binding was created from
    # down into the new BlockEnvironment we just created.
    # This indicates the "declaration trace" to the stack trace
    # mechanisms, which can be different from the "call trace"
    # in the case of, say: eval("caller", a_proc_instance)
    if binding.from_proc?
      be.proc_environment = binding.proc_environment
    end

    be.from_eval!
    be.call
  end
  module_function :eval
  private :eval

  ##
  # :call-seq:
  #   obj.instance_eval(string [, filename [, lineno]] )   => obj
  #   obj.instance_eval {| | block }                       => obj
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
    if ImmediateValue === self
      mc = nil
    else
      mc = Rubinius.object_metaclass(self)
    end

    if prc
      if string
        raise ArgumentError, 'cannot pass both a block and a string to evaluate'
      end
      # Return a copy of the BlockEnvironment with the receiver set to self
      env = prc.block

      if mc
        static_scope = env.method.scope.using_current_as(mc)
      else
        static_scope = env.method.scope.using_disabled_scope
      end

      return env.call_under(self, static_scope, self)
    elsif string
      string = StringValue(string)

      # TODO refactor this common code with #eval
      binding = Binding.setup(Rubinius::VariableScope.of_sender,
                              Rubinius::CompiledMethod.of_sender,
                              Rubinius::StaticScope.of_sender)

      cm = Rubinius::Compiler.compile_eval string, binding.variables, filename, line

      static_scope = binding.static_scope

      if mc
        static_scope = static_scope.using_current_as(mc)
      else
        static_scope = static_scope.using_disabled_scope
      end

      cm.scope = static_scope
      cm.name = :__instance_eval__
      cm.compile

      # This has to be setup so __FILE__ works in eval.
      script = Rubinius::CompiledMethod::Script.new(filename, true)
      cm.scope.script = script

      be = Rubinius::BlockEnvironment.new
      be.from_eval!
      be.under_context binding.variables, cm
      be.call_on_instance(self)
    else
      raise ArgumentError, 'block not supplied'
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
  #   k.instance_exec(5) {|x| @secret+x }   #=> 104

  def instance_exec(*args, &prc)
    raise ArgumentError, "Missing block" unless block_given?
    env = prc.block

    static_scope = env.method.scope
    if ImmediateValue === self
      static_scope = static_scope.using_disabled_scope
    else
      mc = Rubinius.object_metaclass(self)
      static_scope = static_scope.using_current_as(mc)
    end

    return env.call_under(self, static_scope, *args)
  end
end

class Module

  #--
  # These have to be aliases, not methods that call instance eval, because we
  # need to pull in the binding of the person that calls them, not the
  # intermediate binding.
  #++

  def module_eval(string=undefined, filename="(eval)", line=1, &prc)
    # we have a custom version with the prc, rather than using instance_exec
    # so that we can setup the StaticScope properly.
    if prc
      unless string.equal?(undefined)
        raise ArgumentError, "cannot pass both string and proc"
      end

      # Return a copy of the BlockEnvironment with the receiver set to self
      env = prc.block
      static_scope = env.method.scope.using_current_as(self)
      return env.call_under(self, static_scope, self)
    elsif string.equal?(undefined)
      raise ArgumentError, 'block not supplied'
    end

    # TODO refactor this common code with #eval

    variables = Rubinius::VariableScope.of_sender
    method = Rubinius::CompiledMethod.of_sender

    string = StringValue(string)

    cm = Rubinius::Compiler.compile_eval string, variables, filename, line

    # The staticscope of a module_eval CM is the receiver of module_eval
    ss = Rubinius::StaticScope.new self, Rubinius::StaticScope.of_sender

    # This has to be setup so __FILE__ works in eval.
    script = Rubinius::CompiledMethod::Script.new(filename, true)
    ss.script = script

    cm.scope = ss
    cm.compile

    # The gist of this code is that we need the receiver's static scope
    # but the caller's binding to implement the proper constant behavior
    be = Rubinius::BlockEnvironment.new
    be.from_eval!
    be.under_context variables, cm
    be.call_under self, ss, self
  end

  alias_method :class_eval, :module_eval
end
