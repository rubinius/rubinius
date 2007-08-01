class Module
  def module_exec(*args, &prc)
    instance_exec(*args, &prc)
  end
  alias_method :class_exec, :module_exec

  def module_eval(string = nil, filename = "(eval)", lineno = 1, &prc)
    instance_eval(string, filename, lineno, &prc)
  end
  alias_method :class_eval, :module_eval

  # TODO - Handle module_function without args, as per 'private' and 'public'
  def module_function(*method_names)
    if method_names.empty?
      raise ArgumentError, "module_function without an argument is not supported"
    else
      mod_methods = method_table
      inst_methods = metaclass.method_table
      method_names.each do |method_name|
        inst_methods[method_name] = mod_methods[method_name]
      end
    end
    nil
  end
  
  def constants
    constants_table.keys.map { |v| v.to_s }
  end

  def const_set(name, value)
    constants_table[const_name_validate(name)] = value
  end

  def const_get(name)
    constants_table[const_name_validate(name)]
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
