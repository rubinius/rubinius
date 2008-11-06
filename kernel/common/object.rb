class Object

  ##
  # Changes this object's class to +other_klass+.  Thar be dragons.

  def class=(other_klass)
    Ruby.primitive :object_change_class_to
  end

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

  def display(port=$>)
    port.write self
  end
end
