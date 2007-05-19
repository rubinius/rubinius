class Module
  def self.new(&prc)
    mod = allocate
    mod.module_eval(&prc) if block_given?
    mod
  end
  
  def include(*modules)
    modules.reverse_each do |mod|
      mod.append_features(self)
      mod.included(self)
    end
  end

  def to_s
    if name
      name.to_s
    else
      super()
    end
  end

  def __find_method(namesym)
    Ruby.primitive :find_method
  end

  def instance_method(name)
    name = name.to_sym
    cur, cm = __find_method(name)
    return UnboundMethod.new(cur, cm) if cm
    thing = self.kind_of?(Class) ? "class" : "module"
    raise NameError, "undefined method `#{name}' for #{thing} #{self}"
  end

  def instance_methods(all=true)
    methods.names(false)
  end
  
  def const_defined?(name)
    name = name.to_s
    hierarchy = name.split('::')
    hierarchy.shift if hierarchy.first == ""
    hierarchy.shift if hierarchy.first == "Object"
    const = self
    until hierarchy.empty?
      const = const.constants[hierarchy.shift.to_sym]
      return false unless const
    end
    return true
  end

  def define_method(name, meth = nil, &prc)
    meth ||= prc
    case meth
    when Proc
      block_env = meth.block
      meth_ctx = block_env.home
      cm = meth_ctx.method.dup
      initial = block_env.initial_ip
      last = block_env.last_ip + 1
      trimmed_bytecodes = cm.bytecodes.fetch_bytes(initial, last - initial)
      cm.bytecodes = trimmed_bytecodes
    when Method, UnboundMethod
      cm = meth.instance_eval { @method }
      meth = meth.dup
    when CompiledMethod
      cm = meth
      meth = UnboundMethod.new(self, cm)
    else
      raise TypeError, "invalid argument type #{meth.class} (expected Proc/Method)"
    end
    self.methods[name.to_sym] = cm
    meth
  end
  
  def private(*args)
  end
  
  def protected(*args)
  end
  
  def public(*args)
  end

end
