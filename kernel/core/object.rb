class Object

  # VERSION is deprecated in MRI 1.9
  VERSION = Rubinius::RUBY_VERSION
  RUBY_VERSION = Rubinius::RUBY_VERSION
  RUBY_PLATFORM = Rubinius::RUBY_PLATFORM
  RUBY_RELEASE_DATE = Rubinius::RUBY_RELEASE_DATE
  RUBY_PATCHLEVEL = Rubinius::RUBY_PATCHLEVEL
  RUBY_ENGINE = Rubinius::RUBY_ENGINE
  RBX_VERSION = Rubinius::RBX_VERSION

  ivar_as_index :__ivars__ => 0
  def __ivars__; @__ivars__ ; end
  private :__ivars__

  def initialize
  end
  private :initialize

  def undef?
    false
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

  def self.after_loaded
    private :__find_method__
    private :get_instance_variable
    private :get_instance_variables
    private :set_instance_variable
  end

end
