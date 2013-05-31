# -*- encoding: us-ascii -*-

class Binding
  attr_accessor :variables
  attr_accessor :compiled_code
  attr_accessor :constant_scope
  attr_accessor :proc_environment
  attr_accessor :self
  attr_accessor :location

  def from_proc?
    @proc_environment
  end

  # Create a new Binding object. MRI does not allow .new to be called, so
  # we used .setup(). Any code can use this as they wish, provided they have
  # all the right arguments.
  #
  # +variables+ is a Rubinius::VariableScope object
  # +code+ is a Rubinius::CompiledCode object
  # +constant_scope+ is a Rubinius::ConstantScope object
  #
  # See Kernel#binding in kernel/common/eval.rb for a simple example of
  # creating a Binding object.
  #
  def self.setup(variables, code, constant_scope, recv=nil, location=nil)
    bind = allocate()

    bind.self = self_context(recv, variables)
    bind.variables = variables
    bind.compiled_code = code
    bind.constant_scope = constant_scope
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
end
