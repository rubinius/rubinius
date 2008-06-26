# depends on: array.rb proc.rb binding.rb
# (due to alias_method)

module Kernel
  
  def local_variables
    ary = []
    ctx = MethodContext.current.sender
    
    while ctx.kind_of? BlockContext
      if names = ctx.method.local_names
        names.each { |n| ary << n.to_s }
      end
      ctx = ctx.home
    end
        
    if names = ctx.method.local_names
      names.each { |n| ary << n.to_s }
    end
    
    return ary
  end
  module_function :local_variables

  def binding
    ctx = MethodContext.current.sender
    # If we are here because of eval, fetch the context of
    # the thing that invoked eval
    if ctx.from_eval?
      Binding.setup ctx.sender.sender
    else
      Binding.setup ctx
    end
  end
  module_function :binding

  def eval(string, binding=nil, filename='(eval)', lineno=1)
    if !binding
      binding = Binding.setup MethodContext.current.sender
    elsif binding.__kind_of__ Proc
      binding = binding.binding
    elsif !binding.__kind_of__ Binding
      raise ArgumentError, "unknown type of binding"
    end

    compiled_method = Compile.compile_string string, binding.context, filename, lineno
    compiled_method.staticscope = binding.context.method.staticscope.dup

    # This has to be setup so __FILE__ works in eval.
    script = CompiledMethod::Script.new
    script.path = filename
    compiled_method.staticscope.script = script

    be = BlockEnvironment.new
    be.under_context binding.context, compiled_method

    # Pass the BlockEnvironment this binding was created from
    # down into the new BlockEnvironment we just created.
    # This indicates the "declaration trace" to the stack trace
    # mechanisms, which can be different from the "call trace"
    # in the case of, say: eval("caller", a_proc_instance)
    if binding.from_proc? then
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

  def instance_eval(string = nil, filename = "(eval)", line = 1, modeval = false, binding = nil, &prc)
    if prc
      if string
        raise ArgumentError, 'cannot pass both a block and a string to evaluate'
      end
      # Return a copy of the BlockEnvironment with the receiver set to self
      env = prc.block.redirect_to self
      env.method.staticscope = StaticScope.new(__metaclass__, env.method.staticscope)
      original_scope = prc.block.home.method.staticscope
      env.constant_scope = original_scope
      return env.call(*self)
    elsif string
      string = StringValue(string)

      if binding
        context = binding.context
      else
        context = MethodContext.current.sender
      end

      compiled_method = Compile.compile_string string, context, filename, line
      compiled_method.inherit_scope context.method

      # If this is a module_eval style evaluation, add self to the top of the
      # staticscope chain, so that methods and such are added directly to it.
      if modeval
        compiled_method.staticscope = StaticScope.new(self, compiled_method.staticscope)
      else

      # Otherwise add our metaclass, so thats where new methods go.
        compiled_method.staticscope = StaticScope.new(metaclass, compiled_method.staticscope)
      end

      # This has to be setup so __FILE__ works in eval.
      script = CompiledMethod::Script.new
      script.path = filename
      compiled_method.staticscope.script = script

      be = BlockEnvironment.new
      be.from_eval!
      be.under_context context, compiled_method
      be.call_on_instance(self)
    else
      raise ArgumentError, 'block not supplied'
    end
  end

end

class Module

  #--
  # These have to be aliases, not methods that call instance eval, because we
  # need to pull in the binding of the person that calls them, not the
  # intermediate binding.
  #++

  def module_eval(string = Undefined, filename = "(eval)", line = 1, &prc)
    # we have a custom version with the prc, rather than using instance_exec
    # so that we can setup the StaticScope properly.
    if prc
      unless string.equal?(Undefined)
        raise ArgumentError, "cannot pass both string and proc"
      end

      env = prc.block.redirect_to self
      env.method.staticscope = StaticScope.new(self, env.method.staticscope)
      return env.call()
    elsif string.equal?(Undefined)
      raise ArgumentError, 'block not supplied'
    end

    context = MethodContext.current.sender

    string = StringValue(string)

    compiled_method = Compile.compile_string string, context, filename, line

    # The staticscope of a module_eval CM is the receiver of module_eval
    ss = StaticScope.new(self, context.method.staticscope)

    # This has to be setup so __FILE__ works in eval.
    script = CompiledMethod::Script.new
    script.path = filename
    ss.script = script

    compiled_method.staticscope = ss

    # The gist of this code is that we need the receiver's static scope
    # but the caller's binding to implement the proper constant behavior
    be = BlockEnvironment.new
    be.from_eval!
    be.under_context context, compiled_method
    be.make_independent
    be.home.receiver = self
    be.home.make_independent
    # open_module and friends in the VM use this field to determine scope
    be.home.method.staticscope = ss
    be.call
  end
  alias_method :class_eval, :module_eval

  def _eval_under(*args, &block)
    raise "not yet" unless block

    env = block.block.redirect_to self
    env.method.staticscope = StaticScope.new(self, env.method.staticscope)

    return env.call(*args)
  end
  private :_eval_under
end
