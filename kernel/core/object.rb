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
    
  alias_method :eql?, :equal?
  alias_method :==,   :equal?
  alias_method :===,  :equal?
  
  alias_method :__id__, :object_id

  def to_s
    "#<#{self.class.name}:0x#{self.object_id.to_s(16)}>"
  end
  
  def inspect(prefix=nil, vars=nil)
    return "..." if RecursionGuard.inspecting?(self)
    
    return self.to_s unless @__ivars__
    
    if (@__ivars__.is_a?(Hash) or @__ivars__.is_a?(Tuple)) and @__ivars__.empty?
      return self.to_s
    end
    
    prefix = "#{self.class.name}:0x#{self.object_id.to_s(16)}" unless prefix
    parts = []
    
    RecursionGuard.inspect(self) do
    
      if @__ivars__.is_a?(Hash)
        @__ivars__.each do |k,v|
          next if vars and !vars.include?(k)
          parts << "#{k}=#{v.inspect}"
        end
      else
        0.step(@__ivars__.size - 1, 2) do |i|
          if k = @__ivars__[i]
            next if vars and !vars.include?(k)
            v = @__ivars__[i+1]
            parts << "#{k}=#{v.inspect}"
          end
        end
      end
      
    end
    
    if parts.empty?
      "#<#{prefix}>"
    else
      "#<#{prefix} #{parts.join(' ')}>"
    end
  end
  
  alias_method :send, :__send__
  
  def method(name)    
    cm = __find_method__(name)
    
    if cm
      return Method.new(self, cm[1], cm[0])
    else
      raise NoMethodError, "undefined method `#{name}' for #{self.inspect}"
    end
  end
  
  def method_missing(meth, *args)
    # Exclude method_missing from the backtrace since it only confuses
    # people.
    ctx = MethodContext.current.sender
    
    if self.kind_of? Class or self.kind_of? Module
      raise NoMethodError.new("No method '#{meth}' on #{self} (#{self.class})", ctx, args)
    else
      raise NoMethodError.new("No method '#{meth}' on an instance of #{self.class}.", ctx, args)
    end
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
  
  private :get_instance_variable
  
  private :set_instance_variable
  
  def !=(other)
    !(self == other)
  end
  
  # TODO - Implement support for the 'all' parameter
  def singleton_methods(all=true)
    class << self
      (method_table.public_names + method_table.protected_names).map do |name|
        name.to_s
      end
    end
  end

  def private_singleton_methods
    class << self
      method_table.private_names.map { |nam| name.to_s }
    end
  end
  
  def protected_singleton_methods
    class << self
      method_table.protected_names.map { |nam| name.to_s }
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

  def extend(*modules)
    modules.reverse_each do |mod|
      mod.extend_object(self)
    end
    self
  end
end
