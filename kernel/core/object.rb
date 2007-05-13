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
  
  def instance_variables
    vars = get_instance_variables
    return [] if vars.nil?
    return vars.keys.collect { |v| v.to_s }
  end
  
  def get_instance_variables
    Ruby.primitive :ivars_get
  end
  private :get_instance_variables
  
  def instance_variable_get(sym)
    sym = instance_variable_validate(sym)
    get_instance_variable(sym)
  end

  def instance_variable_set(sym, value)
    sym = instance_variable_validate(sym)
    set_instance_variable(sym, value)
  end
  
  # Class variable support
  def class_variable_set(sym, value)
    self.class.class_variable_set sym, value
  end

  def class_variable_get(sym)
    self.class.class_variable_get sym
  end

  def instance_variable_validate(arg)
    # adapted from rb_to_id
    if arg.is_a?(Symbol)
      name = arg.to_s
    elsif arg.is_a?(String)
      name = arg
    elsif arg.is_a?(Fixnum)
      name = arg.id2name
      raise ArgumentError.new("#{arg.inspect} is not a symbol") if arg.nil?
    else
      raise TypeError.new("#{arg.inspect} is not a symbol") unless arg.respond_to?(:to_str)
      name = arg.to_str
    end
    raise NameError.new("`#{arg}' is not allowed as an instance variable name") unless name.to_s[0] == ?@
    name.to_sym
  end
  private :instance_variable_validate
  
  def get_instance_variable(sym)
    Ruby.primitive :ivar_get
  end
  private :get_instance_variable
  
  def set_instance_variable(sym, value)
    Ruby.primitive :ivar_set
  end
  private :set_instance_variable
  
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
  
  def coerce_string
    raise TypeError.new("can't convert #{self.inspect} into String") unless respond_to?(:to_str)
    str = self.to_str
    raise TypeError.new("#{self.class}#to_str should return String") unless str.is_a?(String)
    str
  end
end
