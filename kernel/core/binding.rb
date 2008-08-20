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
  attr_accessor :context
  attr_accessor :proc_environment

  def from_proc?
    @proc_environment
  end

  def self.setup(ctx)
    bind = allocate()
    
    bind.context = ctx
    return bind
  end
end
