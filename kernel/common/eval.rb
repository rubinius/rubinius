# -*- encoding: us-ascii -*-

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
          locals << name.to_s unless locals.include?(name.to_s)
        end
      end

      scope = scope.parent
    end

    Rubinius.convert_to_names(locals)
  end
  module_function :local_variables

  # Obtain binding here for future evaluation/execution context.
  #
  def binding
    return Binding.setup(
      Rubinius::VariableScope.of_sender,
      Rubinius::CompiledMethod.of_sender,
      Rubinius::StaticScope.of_sender,
      self,
      Rubinius::Location.of_closest_ruby_method
    )
  end
  module_function :binding

  # Evaluate and execute code given in the String.
  #
  def eval(string, binding=nil, filename=nil, lineno=1)
    filename = StringValue(filename) if filename
    lineno = Rubinius::Type.coerce_to lineno, Fixnum, :to_i

    if binding
      binding = Rubinius::Type.coerce_to_binding binding
      filename ||= binding.static_scope.active_path
    else
      binding = Binding.setup(Rubinius::VariableScope.of_sender,
                              Rubinius::CompiledMethod.of_sender,
                              Rubinius::StaticScope.of_sender,
                              self)

      filename ||= "(eval)"
    end

    existing_scope = binding.static_scope
    binding.static_scope = existing_scope.dup

    be = Rubinius::Compiler.construct_block string, binding,
                                            filename, lineno

    be.set_eval_binding binding

    result = be.call_on_instance(binding.self)
    binding.static_scope = existing_scope
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
    # so that we can setup the StaticScope properly.
    if prc
      unless string.equal?(undefined)
        raise ArgumentError, "cannot pass both string and proc"
      end

      # Return a copy of the BlockEnvironment with the receiver set to self
      env = prc.block
      static_scope = env.repoint_scope self
      return env.call_under(self, static_scope, self)
    elsif string.equal?(undefined)
      raise ArgumentError, 'block not supplied'
    end

    string = StringValue(string)
    filename = StringValue(filename)

    # The staticscope of a module_eval CM is the receiver of module_eval
    ss = Rubinius::StaticScope.new self, Rubinius::StaticScope.of_sender

    binding = Binding.setup(Rubinius::VariableScope.of_sender,
                            Rubinius::CompiledMethod.of_sender,
                            ss)

    be = Rubinius::Compiler.construct_block string, binding,
                                            filename, line

    be.call_under self, ss, self
  end

  alias_method :class_eval, :module_eval
end
