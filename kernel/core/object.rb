class Object
  
  # VERSION is deprecated in MRI 1.9
  VERSION = Rubinius::RUBY_VERSION
  RUBY_VERSION = Rubinius::RUBY_VERSION
  RUBY_PLATFORM = Rubinius::RUBY_PLATFORM
  RUBY_RELEASE_DATE = Rubinius::RUBY_RELEASE_DATE
  RUBY_ENGINE = Rubinius::RUBY_ENGINE
  RBX_VERSION = Rubinius::RBX_VERSION
  
  ivar_as_index :__ivars__ => 0
  def __ivars__; @__ivars__ ; end

  def initialize
  end
    
  def nil?
    false
  end
  
  def undef?
    false
  end
    
  def instance_of?(cls)
    self.class == cls
  end
  
  alias_method :is_a?, :kind_of?
  
  def dup
    nw = self.class.allocate
    nw.copy_from(self, 0)
    return nw
  end
    
  alias_method :eql?, :equal?
  alias_method :==,   :equal?
  alias_method :===,  :equal?
  
  alias_method :__id__, :object_id

  def to_s
    "#<#{self.class.name}:0x#{self.object_id.to_s(16)}>"
  end
  
  def inspect
    return self.to_s unless @__ivars__
    
    if (@__ivars__.is_a?(Hash) or @__ivars__.is_a?(Tuple)) and @__ivars__.empty?
      return self.to_s
    end
    
    res = "#<#{self.class.name}:0x#{self.object_id.to_s(16)} "
    parts = []
    
    if @__ivars__.is_a?(Hash)    
      @__ivars__.each do |k,v|
        if v.object_id == self.object_id # This would be an infinite loop
          parts << "#{k}=self"
        else
          parts << "#{k}=#{v.inspect}"
        end
      end
    else
      0.step(@__ivars__.size - 1, 2) do |i|
        if k = @__ivars__[i]
          v = @__ivars__[i+1]
          if v.object_id == self.object_id # This would be an infinite loop
            parts << "#{k}=self"
          else
            parts << "#{k}=#{v.inspect}"
          end
        end
      end
    end
    
    res << parts.join(" ")
    res << ">"
    return res
  end
  
  alias_method :send, :__send__
  
  def method(name)    
    cm = __find_method__(name)
    
    if cm
      return Method.new(self, cm[1], cm[0])
    else
      raise NameError, "undefined method `#{name}' for #{self.inspect}"
    end
  end
  
  def lambda(&prc)
    return prc
  end
  
  def method_missing(meth, *args)
    raise NoMethodError, "No method '#{meth}' on an instance of #{self.class}."
  end
  
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
      raise ArgumentError, 'cannot pass both a block and a string to evaluate' if string
      instance_exec(self, &prc)
    elsif string
      cm = string.compile_as_method
      cm.activate(self, [])
    else
      raise ArgumentError, 'block not supplied'
    end
  end
  
  def instance_variables(symbols = false)
    vars = get_instance_variables
    return [] if vars.nil?
    # CSM awareness
    if Tuple === vars
      out = []
      0.step(vars.size - 1, 2) do |i|
        k = vars[i]
        if k
          k = k.to_s unless symbols
          out << k
        else
          return out
        end
      end
      return out
    end
    return vars.keys if symbols
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

  def class_variables(symbols = false)
    self.class.class_variables(symbols)
  end

  def instance_variable_validate(arg)
    # adapted from rb_to_id
    if arg.is_a?(Symbol)
      name = arg.to_s
    elsif arg.is_a?(String)
      name = arg
    elsif arg.is_a?(Fixnum)
      raise ArgumentError.new("#{arg.inspect} is not a symbol")
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

  def !=(other)
    !(self == other)
  end
  
  # TODO - Implement support for the 'all' parameter
  def singleton_methods(all=true)
    class << self
      method_table.public_names
    end
  end

  def private_singleton_methods
    class << self
      method_table.private_names
    end
  end
  
  def protected_singleton_methods
    class << self
      method_table.protected_names
    end
  end

  def methods(all=true)
    names = singleton_methods
    names |= self.class.instance_methods(true) if all
    return names
  end
  
  def public_methods(all=true)
    names = singleton_methods
    names |= self.class.public_instance_methods(all)
    return names
  end
  
  def private_methods(all=true)
    names = private_singleton_methods
    names |= self.class.private_instance_methods(all)
    return names
  end
  
  def protected_methods(all=true)
    names = protected_singleton_methods
    names |= self.class.protected_instance_methods(all)
    return names
  end
end
