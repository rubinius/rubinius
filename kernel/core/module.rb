# depends on: class.rb proc.rb

class Module

  ivar_as_index :__ivars__ => 0, :method_table => 1, :method_cache => 2, :name => 3, :constants => 4, :parent => 5, :superclass => 6
  def method_table   ; @method_table ; end
  def method_cache   ; @method_cache ; end
  def constants_table; @constants    ; end
  def parent         ; @parent       ; end

  def self.new(*args)
    mod = self.allocate

    Rubinius.module_setup_fields(mod)
    block = block_given?
    if block
      mod.initialize(*args, &block)
    else
      mod.initialize(*args)
    end

    mod
  end

  def self.nesting
    mod  = MethodContext.current.sender.receiver
    unless mod.kind_of? Module
      mod = MethodContext.current.sender.method_module
    end
    nesting = []
    while mod != Object && mod.kind_of?(Module)
      nesting << mod
      mod = mod.parent
    end
    nesting
  end

  def initialize
    block = block_given?
    instance_eval(&block) if block
    # I think we need this for constant lookups
    @parent = ::Object
  end

  # HACK: This should work after after the bootstrap is loaded,
  # but it seems to blow things up, so it's only used after
  # core is loaded. I think it's because the bootstrap Class#new
  # doesn't use a privileged send.
  def __method_added__cv(name)
    if name == :initialize
      private :initialize
    end

    method_added(name) if self.respond_to? :method_added
  end

  def verify_class_variable_name(name)
    name = name.kind_of?(Symbol) ? name.to_s : StringValue(name)
    unless name[0..1] == '@@' and name[2].toupper.between?(?A, ?Z) or name[2] == ?_
      raise NameError, "#{name} is not an allowed class variable name"
    end
    name.to_sym
  end
  private :verify_class_variable_name 
    
  def class_variables_table
    @class_variables ||= Hash.new
  end
  private :class_variables_table
  
  def class_variable_set(name, val)
    name = verify_class_variable_name name

    current = direct_superclass
    while current
      vars = current.send :class_variables_table
      return vars[name] = val if vars.key? name
      current = current.direct_superclass
    end
    
    class_variables_table[name] = val
  end

  def class_variable_get(name)
    name = verify_class_variable_name name

    current = self
    while current
      vars = current.send :class_variables_table
      return vars[name] if vars.key? name
      current = current.direct_superclass
    end
    
    raise NameError, "uninitialized class variable #{name} in #{self.name}"
  end
  
  def class_variable_defined?(name)
    name = verify_class_variable_name name
    
    current = self
    while current
      vars = current.send :class_variables_table
      return true if vars.key? name
      current = current.direct_superclass
    end
    return false
  end

  def class_variables(symbols = false)
    names = []
    ancestors.map do |mod|
      names.concat mod.send(:class_variables_table).keys
    end
    names = names.map { |name| name.to_s } unless symbols
    names
  end

  def name
    return @name.to_s if @name
    return calculate_name()
  end

  def to_s
    if @name
      @name.to_s
    else
      super
    end
  end

  def find_method_in_hierarchy(sym)
    if method = @method_table[sym.to_sym]
      method
    elsif direct_superclass
      direct_superclass.find_method_in_hierarchy(sym)
    else
      [Object,Kernel].detect { |k| k.method_table[sym] }
    end
  end

  def ancestors
    if self.class == MetaClass
      out = []
    else
      out = [self]
    end
    sup = direct_superclass()
    while sup
      if sup.class == IncludedModule
        out << sup.module
      elsif sup.class != MetaClass
        out << sup
      end
      sup = sup.direct_superclass()
    end
    return out
  end

  def find_class_method_in_hierarchy(sym)
    self.metaclass.find_method_in_hierarchy(sym)
  end

  def alias_method_cv(new_name, current_name)
    new_name = normalize_name(new_name)
    current_name = normalize_name(current_name)
    meth = find_method_in_hierarchy(current_name)
    if meth
      method_table[new_name] = meth
      VM.reset_method_cache(new_name)
    else
      if self.kind_of? MetaClass
        raise NameError, "Unable to find '#{current_name}' for object #{self.attached_instance.inspect}"
      else
        thing = self.kind_of?(Class) ? "class" : "module"
        raise NameError, "undefined method `#{current_name}' for #{thing} `#{self.name}'"
      end
    end
  end

  def remote_alias(new_name, mod, current_name)
    cm = mod.find_method_in_hierarchy(current_name)
    unless cm
      raise NameError, "Unable to find method '#{current_name}' under #{mod}"
    end

    if cm.kind_of? Tuple
      meth = cm[1]
    else
      meth = cm
    end

    if meth.primitive and meth.primitive > 0
      raise NameError, "Unable to remote alias primitive method '#{current_name}'"
    end

    method_table[new_name] = cm
    VM.reset_method_cache(new_name)

    return new_name
  end


  def undef_method(*names)
    names.each do |name|
      # Will raise a NameError if the method doesn't exist.
      instance_method(name)
      method_table[name] = false
      VM.reset_method_cache(name)

      method_undefined(name) if respond_to? :method_undefined
    end

    nil
  end

  def remove_method(*names)
    names.each do |name|
      instance_method(name)
      method_table.delete name
      VM.reset_method_cache(name)

      method_removed(name) if respond_to? :method_removed
    end

    nil
  end

  def public_method_defined?(sym)
    sym = StringValue(sym) unless sym.is_a? Symbol
    m = find_method_in_hierarchy sym
    m ? m.first == :public : false
  end

  def private_method_defined?(sym)
    sym = StringValue(sym) unless sym.is_a? Symbol
    m = find_method_in_hierarchy sym
    m ? m.first == :private : false
  end

  def protected_method_defined?(sym)
    sym = StringValue(sym) unless sym.is_a? Symbol
    m = find_method_in_hierarchy sym
    m ? m.first == :protected : false
  end

  def method_defined?(sym)
    sym = normalize_name(sym)
    m = find_method_in_hierarchy sym
    m ? [:public,:protected].include?(m.first) : false
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
    unless all or self.is_a?(MetaClass) or self.is_a?(IncludedModule)
      return names.map { |name| name.to_s }
    end

    excludes = method_table.map { |name, meth| meth == false ? name : nil }
    undefed = excludes.compact

    sup = direct_superclass

    while sup
      names |= sup.method_table.__send__(filter)

      excludes = method_table.map { |name, meth| meth == false ? name : nil }
      undefed += excludes.compact

      sup = sup.direct_superclass
    end

    (names - undefed).map { |name| name.to_s }
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

  def extend_object(obj)
    obj.metaclass.include self
  end

  # Don't call this include, otherwise it will shadow the bootstrap
  # version while core loads (a violation of the core/bootstrap boundry)
  def include_cv(*modules)
    modules.reverse_each do |mod|
      raise TypeError, "wrong argument type #{mod.class} (expected Module)" unless mod.class == Module
      next if ancestors.include?(mod)
      mod.send(:append_features, self)
      mod.send(:included, self)
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

  def included_modules
    out = []
    sup = direct_superclass

    while sup
      if sup.class == IncludedModule
        out << sup.module
      end

      sup = sup.direct_superclass
    end

    out
  end

  def set_visibility(meth, vis)
    name = normalize_name(meth)
    tup = find_method_in_hierarchy(name)
    vis = vis.to_sym

    unless tup
      raise NoMethodError, "Unknown method '#{name}' to make #{vis.to_s}"
    end

    method_table[name] = tup.dup
    if tup.kind_of?(Tuple)
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

    if tup.kind_of?(Tuple)
      mc.method_table[name] = tup.dup
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

  def module_exec(*args, &prc)
    instance_exec(*args, &prc)
  end
  alias_method :class_exec, :module_exec

  # TODO - Handle module_function without args, as per 'private' and 'public'
  def module_function_cv(*args)
    if args.empty?
      raise ArgumentError, "module_function without an argument is not supported"
    else
      mc = self.metaclass
      args.each do |meth|
        method_name = normalize_name(meth)
        method = find_method_in_hierarchy(method_name)
        mc.method_table[method_name] = method.dup
        set_visibility(method_name, :private)
      end
      private *args
    end
    nil
  end

  def constants
    constants = self.constants_table.keys
    current = self.direct_superclass

    while current != nil && current != Object
      constants += current.constants_table.keys
      current = current.direct_superclass
    end

    constants.map { |c| c.to_s }
  end

  def const_defined?(name)
    name = normalize_const_name(name)

    current = self
    while current
      return true if current.constants_table.has_key?(name)
      current = current.direct_superclass
    end

    return false
  end

  def const_set(name, value)
    if value.is_a? Module
      value.set_name_if_necessary(name, self)
    end
    constants_table[normalize_const_name(name)] = value
  end

  def const_get(name)
    recursive_const_get(name)
  end

  def const_missing(name)
    raise NameError, "uninitialized constant #{name}"
  end

  def attr_reader_cv(*names)
    names.each do |name|
      method_symbol = reader_method_symbol(name)
      access_method = AccessVarMethod.get_ivar(attribute_symbol(name))
      self.method_table[method_symbol] = access_method
    end

    return nil
  end

  def attr_writer_cv(*names)
    names.each do |name|
      method_symbol = writer_method_symbol(name)
      access_method = AccessVarMethod.set_ivar(attribute_symbol(name))
      self.method_table[method_symbol] = access_method
    end

    return nil
  end

  def attr_accessor_cv(*names)
    names.each do |name|
      attr(name,true)
    end

    return nil
  end

  def attr(name,writeable=false)
    attr_reader(name)
    attr_writer(name) if writeable
    return nil
  end

  def <(other)
    unless other.kind_of? Module
      raise TypeError, "compared with non class/module"
    end
    return false if self.equal? other
    ancestors.index(other) && true
  end

  def <=(other)
    return true if self.equal? other
    lt = self < other
    return false if lt.nil? && other < self
    lt
  end

  def >(other)
    unless other.kind_of? Module
      raise TypeError, "compared with non class/module"
    end
    other < self
  end

  def >=(other)
    unless other.kind_of? Module
      raise TypeError, "compared with non class/module"
    end
    return true if self.equal? other
    gt = self > other
    return false if gt.nil? && other > self
    gt
  end

  def <=>(other)
    return 0 if self.equal? other
    return nil unless other.kind_of? Module
    lt = self < other
    if lt.nil?
      other < self ? 1 : nil
    else
      lt ? -1 : 1
    end
  end

  def ===(inst)
    return true if inst.kind_of? self
    # TODO: check if inst is extended by self
    # inst.metaclass < self & true rescue false
    false
  end

  def set_name_if_necessary(name, mod)
    return unless @name.nil?
    name = name.to_s.dup
    while mod and mod != Object
      name.insert(0, "#{mod.name}::")
      mod = mod.parent
    end
    @name = name.to_sym
  end

  # A fixup, move the core versions in place now that everything
  # is loaded.
  def self.after_loaded
    alias_method :__method_added__, :__method_added__cv
    alias_method :alias_method, :alias_method_cv
    alias_method :module_function, :module_function_cv
    alias_method :include, :include_cv
    alias_method :private, :private_cv
    alias_method :append_features, :append_features_cv
    alias_method :attr_reader, :attr_reader_cv
    alias_method :attr_writer, :attr_writer_cv
    alias_method :attr_accessor, :attr_accessor_cv
  end

  def autoload(name, path)
    name = normalize_const_name(name)
    raise ArgumentError, "empty file name" if path.empty?
    @autoloads ||= Hash.new
    @autoloads[name] = path
  end

private

  def calculate_name
    # This should be removed if/when constant assignment happens
    # via const_set() - it's pretty ugly!

    seen = { Object => true }
    constants = [[Object, Object.constants_table]]

    until constants.empty?
      mod, table = constants.shift
      table.each do |const_name, value|
        if value.equal? self
          set_name_if_necessary(const_name.to_s, mod)
          return @name.to_s
        elsif value.is_a? Module
          unless seen[value]
            constants << [value, value.constants_table]
            seen[value] = true
          end
        end
      end
    end

    return ""
  end

  def remove_const(name)
    const_missing(name) unless constants_table.has_key?(name)
    constants_table.delete(name)
  end

  def extended(name)
    nil
  end

  def method_added(name)
    nil
  end

  def normalize_name(name)
    sym_name = nil
    if name.respond_to?(:to_sym)
      warn 'do not use Fixnums as Symbols' if name.kind_of?(Fixnum)
      sym_name = name.to_sym
    elsif name.respond_to?(:to_str)
      sym_name = StringValue(name).to_sym
    end
    raise TypeError, "#{name} is not a symbol" unless sym_name

    sym_name
  end

  # Get a constant with the given name. If the constant does not exist, return nil.
  def recursive_const_get(name)
    name = normalize_const_name(name)

    constant = nil
    current = self
    while current
      return constant if constant = current.constants_table[name]
      current = current.direct_superclass
    end

    if @autoloads and path = @autoloads[name]
      require path
      @autoloads.delete(name)
      return recursive_const_get(name)
    end

    const_missing(name)
  end

  def normalize_const_name(name)
    name = normalize_name(name)
    raise NameError, "wrong constant name #{name}" unless valid_const_name?(name)
    name
  end

  # Modified to fit definition at:
  # http://docs.huihoo.com/ruby/ruby-man-1.4/syntax.html#variable
  def valid_const_name?(name)
    name.to_s =~ /^((::)?[A-Z]\w*)+$/ ? true : false
  end

  def attribute_symbol(name)
    "@#{normalize_name(name)}".to_sym
  end

  def reader_method_symbol(name)
    normalize_name(name)
  end

  def writer_method_symbol(name)
    "#{normalize_name(name)}=".to_sym
  end

end
