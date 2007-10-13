class Module
  
  ivar_as_index :__ivars__ => 0, :method_table => 1, :method_cache => 2, :name => 3, :constants => 4, :parent => 5, :superclass => 6
  def method_table   ; @method_table ; end
  def method_cache   ; @method_cache ; end
  def constants_table; @constants    ; end
  def parent         ; @parent       ; end
  
  def to_s
    if @name
      @name.to_s
    else
      super
    end
  end
  
  def find_method_in_hierarchy(sym)
    @method_table[sym] || Object.find_method_in_hierarchy(sym)
  end
  
  def ancestors
    out = [self]
    sup = direct_superclass()
    while sup
      if sup.class == IncludedModule
        out << sup.module
      else
        out << sup
      end
      sup = sup.direct_superclass()
    end
    return out
  end
  
  def find_class_method_in_hierarchy(sym)
    mc = self.metaclass
    mc.method_table[sym] || mc.find_method_in_hierarchy(sym)
  end

  def alias_method(new_name, current_name)
    meth = find_method_in_hierarchy(current_name)
    if meth
      method_table[new_name] = meth
      VM.reset_method_cache(new_name)
    else
      raise NameError, "undefined method `#{current_name}' for module `#{self.name}'"
    end
  end
  
  def instance_method(name)
    name = name.to_sym
    cur, cm = __find_method(name)
    return UnboundMethod.new(cur, cm) if cm
    thing = self.kind_of?(Class) ? "class" : "module"
    raise NameError, "undefined method `#{name}' for #{thing} #{self}"
  end

  def instance_methods(all=true)
    filter_methods(:public_names, all) | filter_methods(:protected_names, all)
  end

  def public_instance_methods(all=true)
    filter_methods(:public_names, all)
  end
  
  def private_instance_methods(all=true)
    filter_methods(:private_names, all)
  end
  
  def protected_instance_methods(all=true)
    filter_methods(:protected_names, all)
  end
  
  def filter_methods(filter, all)
    names = method_table.__send__(filter)
    # TODO: fix for module when modules can include modules
    return names if self.is_a?(Module)
    unless all or self.is_a?(MetaClass) or self.is_a?(IncludedModule)
      return names
    end
    
    sup = direct_superclass()
    while sup
      names |= sup.method_table.__send__(filter)
      sup = sup.direct_superclass()
    end
    
    return names
  end
  # private :filter_methods
  
  def define_method(name, meth = nil, &prc)
    meth ||= prc

    if meth.kind_of?(Proc)
      block_env = meth.block
      cm = DelegatedMethod.build(:call_on_instance, block_env, true)
    elsif meth.kind_of?(Method)
      cm = DelegatedMethod.build(:call, meth, false)
    elsif meth.kind_of?(UnboundMethod)
      cm = DelegatedMethod.build(:call_on_instance, meth, true)
    else
      raise TypeError, "wrong argument type #{meth.class} (expected Proc/Method)"
    end

    self.method_table[name.to_sym] = cm
    VM.reset_method_cache(name.to_sym)
    meth
  end
  
  # Don't call this include, otherwise it will shadow the bootstrap
  # version while core loads (a violation of the core/bootstrap boundry)
  def include_cv(*modules)
    modules.reverse_each do |mod|
      raise TypeError, "wrong argument type #{mod.class} (expected Module)" unless mod.class == Module
      mod.append_features(self)
      mod.included(self)
    end
  end
  
  def append_features_cv(mod)
    ancestors.reverse_each do |m|
      im = IncludedModule.new(m)
      im.attach_to mod
    end
  end
  
  def include?(mod)
    raise TypeError, "wrong argument type #{mod.class} (expected Module)" unless mod.class == Module
    ancestors.include? mod
  end
  
  def set_visibility(meth, vis)
    name = meth.to_sym
    tup = find_method_in_hierarchy(name)
    vis = vis.to_sym
    
    unless tup
      raise NoMethodError, "Unknown method '#{name}' to make private"
    end

    method_table[name] = tup.dup
    if Tuple === tup
      method_table[name][0] = vis
    else
      method_table[name] = Tuple[vis, tup]
    end
    
    return name
  end
  
  def set_class_visibility(meth, vis)
    name = meth.to_sym
    tup = find_class_method_in_hierarchy(name)
    vis = vis.to_sym
    
    unless tup
      raise NoMethodError, "Unknown class method '#{name}' to make #{vis.to_s}"
    end
    
    mc = self.metaclass
    mc.method_table[name] = tup.dup

    if Tuple === tup
      mc.method_table[name][0] = vis
    else
      mc.method_table[name] = Tuple[vis, tup]
    end
    return name
  end

  # Same as include_cv above, don't call this private.
  def private_cv(*args)
    args.each { |meth| set_visibility(meth, :private) }
  end
  
  def protected(*args)
    args.each { |meth| set_visibility(meth, :protected) }
  end
  
  def public(*args)
    args.each { |meth| set_visibility(meth, :public) }
  end
  
  def private_class_method(*args)
    args.each { |meth| set_class_visibility(meth, :private) }
    self
  end

  def public_class_method(*args)
    args.each { |meth| set_class_visibility(meth, :public) }
    self
  end

  # A fixup, move the core versions in place now that everything
  # is loaded.
  def self.after_loaded
    alias_method :include, :include_cv
    alias_method :private, :private_cv
    alias_method :append_features, :append_features_cv
  end
  
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
  
  def const_defined?(name)
    recursive_const_get(name) ? true : false
  end

  def const_set(name, value)
    constants_table[normalize_const_name(name)] = value
  end

  def const_get(name)
    const = recursive_const_get(name)
    unless const
      raise NameError, "uninitialized constant #{self.to_s}::#{name}"
    end
    const
  end
  
  def const_missing(name)
    raise NameError, "Unable to find constant #{name}" 
  end
  

  #
  # The following method will be made private when attr is moved from Class to Module.
  # Right now, Class uses this methods.
  # 
  def normalize_name(name)
    sym_name = nil
    
    if name.respond_to?(:to_sym)
      warn 'do not use Fixnums as Symbols' if name.kind_of?(Fixnum)
      sym_name = name.to_sym
    elsif name.respond_to?(:to_str)
      sym_name = name.to_str
      raise TypeError, "Object#to_str should return String" unless sym_name.kind_of?(String)
      sym_name = sym_name.to_sym
    end
    
    raise TypeError, "#{name} is not a symbol" if sym_name == nil
    
    sym_name
  end

private

  # Get a constant with the given name. If the constant does not exist, return nil.
  def recursive_const_get(name)
    if name.kind_of?(String)
      hierarchy = name.split("::")
      hierarchy.shift if hierarchy.first == ""
      hierarchy.shift if hierarchy.first == "Object"
      
      const = self
      hierarchy.each do |c|
        const = const.constants_table[normalize_const_name(c)]
        return nil unless const
      end
    else
      const = constants_table[normalize_const_name(name)]
    end
    const
  end
  
  def normalize_const_name(name)
    name = normalize_name(name)
    unless valid_const_name?(name)
      raise NameError, "wrong constant name #{name}"
    end
    name
  end
  
  def valid_const_name?(name)
    name.to_s =~ /^[A-Z]\w*$/ ? true : false
  end
end
