class Object
  #  call-seq:
  #     obj.instance_exec(arg...) {|var...| block }                       => obj
  #  
  #  Executes the given block within the context of the receiver
  #  (_obj_). In order to set the context, the variable +self+ is set
  #  to _obj_ while the code is executing, giving the code access to
  #  _obj_'s instance variables.  Arguments are passed as block parameters.
  #     
  #     class Klass
  #       def initialize
  #         @secret = 99
  #       end
  #     end
  #     k = Klass.new
  #     k.instance_exec(5) {|x| @secret+x }   #=> 104
  def instance_exec(*args, &prc)
    raise ArgumentError, "Missing block" unless block_given?
    env = prc.block.dup
    env.put(1, env.home.dup) # home: MethodContext
    env.home.put(9, self) # receiver: Object
    proc = Proc.from_environment(env)
    proc.call(*args)
  end

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
    if block_given?
      instance_exec(&prc)
    else
      cm = string.compile_as_method
      cm.activate(self, [])
    end
  end
  
  def instance_variables
    if !@__ivars__ or @__ivars__.size == 0
      return []
    end
    res = []
    @__ivars__.each do |k,v|
      res << k.to_s
    end
    return res
  end
  
  def Float(obj)
    return obj.to_f if obj.respond_to?(:to_f)
    raise TypeError, "can't convert #{obj.class} into Float"
  end
end