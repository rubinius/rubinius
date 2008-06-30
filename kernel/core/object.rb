class Object

  #--
  # deprecated in MRI 1.9
  #++

  VERSION = Rubinius::RUBY_VERSION
  RUBY_VERSION = Rubinius::RUBY_VERSION
  RUBY_PATCHLEVEL = Rubinius::RUBY_PATCHLEVEL
  RUBY_ENGINE = Rubinius::RUBY_ENGINE
  RBX_VERSION = Rubinius::RBX_VERSION

  ivar_as_index :__ivars__ => 0

  def __ivars__; @__ivars__ ; end
  private :__ivars__

  def initialize
  end
  private :initialize

  def instance_variable_validate(name)
    # adapted from rb_to_id
    case name
    when Symbol
      return name if name.to_s[0] == ?@
    when String
      return name.intern if name[0] == ?@
    when Fixnum
      raise ArgumentError.new("#{name.inspect} is not a symbol")
    else
      raise TypeError.new("#{name.inspect} is not a symbol") unless name.respond_to?(:to_str)
      name = name.to_str
      return name.to_sym if name[0] == ?@
    end
    
    raise NameError.new("`#{name}' is not allowed as an instance variable name")
  end
  private :instance_variable_validate

  def self.after_loaded
    private :__find_method__
    private :get_instance_variable
    private :get_instance_variables
    private :set_instance_variable
  end

  def display(port=$>)
    port.write self
  end
end
