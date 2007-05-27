class Module
  def module_exec(*args, &prc)
    instance_exec(*args, &prc)
  end
  alias_method :class_exec, :module_exec

  def module_eval(string = nil, filename = "(eval)", lineno = 1, &prc)
    instance_eval(string, filename, lineno, &prc)
  end
  alias_method :class_eval, :module_eval

  def const_set(name, value)
    constants[const_name_validate(name)] = value
  end

  def const_get(name)
    constants[const_name_validate(name)]
  end

  def const_name_validate(name)
    raise ArgumentError, "#{name} is not a symbol" if Fixnum === name

    unless name.respond_to?(:to_sym)
      raise TypeError, "#{name} is not a symbol" 
    end

    unless name.to_s =~ /^[A-Z]\w*$/
      raise NameError, "wrong constant name #{name}"
    end

    name.to_sym
  end
  private :const_name_validate
end
