# -*- encoding: us-ascii -*-

class Binding
  attr_accessor :variables
  attr_accessor :code
  attr_accessor :static_scope
  attr_accessor :proc_environment
  attr_accessor :self
  attr_accessor :line_number

  def from_proc?
    @proc_environment
  end

  # Create a new Binding object. MRI does not allow .new to be called, so
  # we used .setup(). Any code can use this as they wish, provided they have
  # all the right arguments.
  #
  # +variables+ is a Rubinius::VariableScope object
  # +code+ is a Rubinius::CompiledMethod object
  # +static_scope+ is a Rubinius::StaticScope object
  #
  # See Kernel#binding in kernel/common/eval.rb for a simple example of
  # creating a Binding object.
  #
  def self.setup(variables, code, static_scope, recv=nil, location=nil)
    bind = allocate()

    bind.self = recv || variables.self
    bind.variables = variables
    bind.code = code
    bind.static_scope = static_scope
    bind.line_number = location ? location.line : 1

    return bind
  end

  # Evaluates the Ruby expression(s) in string, in the binding‘s context.
  # If the optional filename and lineno parameters are present,
  # they will be used when reporting syntax errors.
  def eval(expr, filename=nil, lineno=nil)
    lineno ||= filename ? 1 : self.line_number

    Kernel.eval(expr, self, filename, lineno)
  end
end
