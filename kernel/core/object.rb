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
    env = prc.block.redirect_to self
    env.call(*args)
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
  
  def instance_variables?
    @__ivars__ and @__ivars__.size > 0
  end
  private :instance_variables?
  
  def instance_variables
    res = []
    if instance_variables?
      @__ivars__.each do |k,v|
        res << k.to_s
      end
    end
    return res
  end
  
  def instance_variable_argument_valid?(arg)
    raise TypeError.new("#{arg.inspect} is not a symbol") unless Symbol === arg or String === arg
    raise NameError.new("`#{arg}' is not allowed as an instance variable name") unless arg.to_s[0] == ?@
  end
  private :instance_variable_argument_valid?
  
  def instance_variable_get(sym)
    Ruby.primitive :ivar_get
    instance_variable_argument_valid?(sym)
    raise TypeError.new("Unable to get instance variable #{sym} on #{self.inspect}")
  end

  def instance_variable_set(sym, value)
    Ruby.primitive :ivar_set
    instance_variable_argument_valid?(sym)
    raise TypeError.new("Unable to set instance variable #{sym} on #{self.inspect}")
  end
  
  def taint
    Ruby.primitive :object_taint
  end
  
  def tainted?
    Ruby.primitive :object_tainted_p
  end
  
  def freeze
    Ruby.primitive :object_freeze
  end
  
  def frozen?
    Ruby.primitive :object_frozen_p
  end

  def to_a
    if self.kind_of? Array
      self
    else
      [self]
    end
  end

  def ==(other)
    self.object_id == other.object_id
  end

  def __not_equal__(other)
    !(self == other)
  end
  self.methods[:"!="] = self.methods[:__not_equal__]
end
