##
# Objects of class Binding encapsulate the execution context at some
# particular place in the code and retain this context for future use. The
# variables, methods, value of self, and possibly an iterator block that can
# be accessed in this context are all retained. Binding objects can be created
# using Kernel#binding, and are made available to the callback of
# Kernel#set_trace_func.
#
# These binding objects can be passed as the second argument of the
# Kernel#eval method, establishing an environment for the evaluation.
#
#   class Demo
#     def initialize(n)
#       @secret = n
#     end
#     def getBinding
#       return binding()
#     end
#   end
#
#   k1 = Demo.new(99)
#   b1 = k1.getBinding
#   k2 = Demo.new(-3)
#   b2 = k2.getBinding
#
#   eval("@secret", b1)   #=> 99
#   eval("@secret", b2)   #=> -3
#   eval("@secret")       #=> nil
#
# Binding objects have no class-specific methods.

class Binding
  attr_accessor :variables
  attr_accessor :code
  attr_accessor :static_scope
  attr_accessor :proc_environment
  attr_accessor :self

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
  def self.setup(variables, code, static_scope, recv=nil)
    bind = allocate()

    bind.self = recv || variables.self
    bind.variables = variables
    bind.code = code
    bind.static_scope = static_scope
    return bind
  end

  # Evaluates the Ruby expression(s) in string, in the binding‘s context.
  # If the optional filename and lineno parameters are present,
  # they will be used when reporting syntax errors.
  def eval(expr, *arg)
    Kernel.eval(expr, self, *arg)
  end
end
