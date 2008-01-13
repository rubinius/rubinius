# depends on: array.rb
#
# The dependency is from #alias_method.

class Binding
  def self.setup(ctx)
    bind = allocate()
    if ctx.kind_of? BlockContext
      while ctx.env.from_eval?
        ctx = ctx.env.home_block
      end
    end
    
    bind.context = ctx
    return bind
  end
  
  attr_accessor :context
end

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
    Binding.setup MethodContext.current.sender
  end
  module_function :binding
  
  def eval(string, binding=nil, filename='(eval)', lineno=1)
    if !binding
      binding = Binding.setup MethodContext.current.sender
    elsif binding.kind_of? Proc
      binding = Binding.setup binding.block.home_block
    elsif !binding.kind_of? Binding
      raise ArgumentError, "unknown type of binding"
    end

    flags = { :binding => binding }
    compiled_method = Compile.compile_string string, flags, filename, lineno
    compiled_method.inherit_scope binding.context.method
    ctx = binding.context
    be = BlockEnvironment.new
    be.from_eval!
    be.under_context ctx, compiled_method
    be.call
  end
  
  private :eval
  
  #  call-seq:
  #     obj.instance_eval(string [, filename [, lineno]] )   => obj
  #     obj.instance_eval {| | block }                       => obj
  #  
  #  Evaluates a string containing Ruby source code, or the given block,
  #  within the context of the receiver (_obj_). In order to set the
  #  context, the variable +self+ is set to _obj_ while
  #  the code is executing, giving the code access to _obj_'s
  #  instance variables. In the version of <code>instance_eval</code>
  #  that takes a +String+, the optional second and third
  #  parameters supply a filename and starting line number that are used
  #  when reporting compilation errors.
  #     
  #     class Klass
  #       def initialize
  #         @secret = 99
  #       end
  #     end
  #     k = Klass.new
  #     k.instance_eval { @secret }   #=> 99
  def instance_eval(string = nil, filename = "(eval)", line = 1, &prc)
    if prc
      if string
        raise ArgumentError, 'cannot pass both a block and a string to evaluate'
      end
      return instance_exec(self, &prc)
    elsif string
      string = StringValue(string)
      
      binding = Binding.setup(MethodContext.current.sender)
 
      flags = { :binding => binding }
      compiled_method = Compile.compile_string string, flags, filename, line
      ctx = binding.context
      be = BlockEnvironment.new
      be.from_eval!
      be.under_context ctx, compiled_method
      be.call_on_instance(self)
    else
      raise ArgumentError, 'block not supplied'
    end
  end
  
  
end

class Module
  # These have to be aliases, not methods that call instance eval, because we
  # need to pull in the binding of the person that calls them, not the intermediate
  # binding.
  alias_method :module_eval, :instance_eval
  alias_method :class_eval, :module_eval
end
