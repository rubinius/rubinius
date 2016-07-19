class Binding
  attr_accessor :variables
  attr_accessor :compiled_code
  attr_accessor :lexical_scope
  attr_accessor :proc_environment
  attr_accessor :location
  attr_accessor :receiver

  def from_proc?
    @proc_environment
  end

  def self.self_context(recv, variables)
    Rubinius::Type.object_equal(recv, Kernel) ? recv : variables.self
  end
  private :self_context

  # Create a new Binding object. MRI does not allow .new to be called, so
  # we used .setup(). Any code can use this as they wish, provided they have
  # all the right arguments.
  #
  # +variables+ is a Rubinius::VariableScope object
  # +code+ is a Rubinius::CompiledCode object
  # +lexical_scope+ is a Rubinius::LexicalScope object
  #
  # See Kernel#binding in core/eval.rb for a simple example of creating a
  # Binding object.
  #
  def self.setup(variables, code, lexical_scope, recv=nil, location=nil)
    bind = allocate()

    bind.receiver = self_context(recv, variables)
    bind.variables = variables
    bind.compiled_code = code
    bind.lexical_scope = lexical_scope
    bind.location = location

    return bind
  end

  def line_number
    if proc_environment
      proc_environment.line
    else
      location ? location.line : 1
    end
  end

  # Evaluates the Ruby expression(s) in string, in the binding‘s context.
  # If the optional filename and lineno parameters are present,
  # they will be used when reporting syntax errors.
  def eval(expr, filename=nil, lineno=nil)
    lineno ||= filename ? 1 : line_number

    Kernel.eval(expr, self, filename, lineno)
  end

  def local_variables
    variables.local_variables
  end

  def local_variable_set(name, value)
    unless name.is_a?(Symbol)
      name = Rubinius::Type.coerce_to(name, String, :to_str).to_sym
    end

    vars = variables

    # If a local variable is defined in a parent scope we should update the
    # variable in said scope and all child scopes, instead of _only_ setting it
    # in the current scope.
    while vars
      meth = vars.method

      if meth.local_names.include?(name)
        return vars.set_local(meth.local_slot(name), value)
      elsif vars.eval_local_defined?(name)
        return vars.set_eval_local(name, value)
      end

      vars = vars.parent
    end

    variables.set_eval_local(name, value)
  end

  def local_variable_get(name)
    unless name.is_a?(Symbol)
      name = Rubinius::Type.coerce_to(name, String, :to_str).to_sym
    end

    vars = variables

    while vars
      meth = vars.method

      if meth.local_names.include?(name)
        return vars.locals[meth.local_slot(name)]
      elsif vars.eval_local_defined?(name)
        return vars.get_eval_local(name)
      end

      vars = vars.parent
    end

    raise NameErrror, "local variable #{name.inspect} not defined for #{inspect}"
  end

  def local_variable_defined?(name)
    unless name.is_a?(Symbol)
      name = Rubinius::Type.coerce_to(name, String, :to_str).to_sym
    end

    variables.local_defined?(name)
  end
end
