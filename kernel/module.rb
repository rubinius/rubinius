class Module
  
  def self.new(&prc)
    mod = allocate
    if block_given?
      mod.instance_eval(&prc)
    end
    mod
  end

  def to_s
    if name
      name.to_s
    else
      super()
    end
  end

  def module_eval(string = nil, filename = nil, lineno = 0, &prc)
    instance_eval(string, filename, lineno, &prc)
  end

  alias_method :class_eval, :module_eval

  def instance_method(name)
    name = name.to_sym
    cur  = self
    while cur
      cm = cur.methods[name]
      return UnboundMethod.new(cur, cm) if cm
      cur = cur.direct_superclass
    end
    thing = self.kind_of?(Class) ? "class" : "module"
    raise NameError, "undefined method `#{name}' for #{thing} #{self}"
  end

end
