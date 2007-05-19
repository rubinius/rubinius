class Module
  def module_exec(*args, &prc)
    instance_exec(*args, &prc)
  end
  alias_method :class_exec, :module_exec

  def module_eval(string = nil, filename = "(eval)", lineno = 1, &prc)
    instance_eval(string, filename, lineno, &prc)
  end
  alias_method :class_eval, :module_eval
end
