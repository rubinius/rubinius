##
# Some terminology notes:
#
# [Encloser] The Class or Module inside which this one is defined or, in the
#            event we are at top-level, Object.
#
# [Direct superclass] Whatever is next in the chain of superclass invocations.
#                     This may be either an included Module, a Class or nil.
#
# [Superclass] The real semantic superclass and thus only applies to Class
#              objects.

class Module

  def constants_table() ; @constants ; end
  attr_writer :method_table

  private :included

  def self.nesting
    # TODO: this is not totally correct but better specs need to
    # be written. Until then, this gets the specs running without
    # choking on MethodContext
    scope = Rubinius::CompiledMethod.of_sender.scope
    nesting = []
    while scope and scope.module != Object
      nesting << scope.module
      scope = scope.parent
    end
    nesting
  end

  def initialize(&block)
    # The method and constants tables are created and setup
    # by Class and Module allocate.
    module_eval(&block) if block
  end

  def verify_class_variable_name(name)
    if name.kind_of? Symbol
      return name if name.is_cvar?
      raise NameError, "#{name} is not an allowed class variable name"
    end

    name = StringValue(name)
    unless name[0..1] == '@@' and name[2].toupper.between?(?A, ?Z) or name[2] == ?_
      raise NameError, "#{name} is not an allowed class variable name"
    end
    name.to_sym
  end
  private :verify_class_variable_name


  def class_variable_set(name, val)
    Ruby.primitive :module_cvar_set

    class_variable_set verify_class_variable_name(name), val
  end

  def class_variable_get(name)
    Ruby.primitive :module_cvar_get

    class_variable_get verify_class_variable_name(name)
  end

  def class_variable_defined?(name)
    Ruby.primitive :module_cvar_defined

    class_variable_defined? verify_class_variable_name(name)
  end

  def remove_class_variable(name)
    Ruby.primitive :module_cvar_remove

    remove_class_variable verify_class_variable_name(name)
  end
  private :remove_class_variable

  def __class_variables__
    Ruby.primitive :module_class_variables

    raise PrimitiveFailure, "module_class_variables failed"
  end

  def class_variables
    Rubinius.convert_to_names(__class_variables__)
  end

  def name
    @module_name ? @module_name.to_s : ""
  end

  alias_method :__name__, :name

  def __path__
    return @module_name if @module_name
    inspect
  end

  def to_s
    @module_name ? @module_name.to_s : super
  end

  alias_method :inspect, :to_s

  def lookup_method(sym, check_object_too=true, trim_im=true)
    mod = self

    while mod
      if entry = mod.method_table.lookup(sym.to_sym)
        mod = mod.module if trim_im and mod.kind_of? Rubinius::IncludedModule
        return [mod, entry]
      end

      mod = mod.direct_superclass
    end

    # Optionally also search Object (and everything included in Object);
    # this lets a module alias methods on Object or Kernel.
    if check_object_too and instance_of?(Module)
      return Object.lookup_method(sym, true, trim_im)
    end
  end

  def _each_ancestor
    if kind_of? Class and __metaclass_object__
      # nothing
    else
      yield self
    end

    sup = direct_superclass()
    while sup
      if sup.kind_of? Rubinius::IncludedModule
        yield sup.module
      elsif sup.kind_of? Class
        yield sup unless sup.__metaclass_object__
      else
        yield sup
      end
      sup = sup.direct_superclass()
    end
  end

  private :_each_ancestor

  def ancestors
    out = []
    _each_ancestor { |mod| out << mod }
    return out
  end


  def superclass_chain
    out = []
    mod = direct_superclass()
    while mod
      out << mod
      mod = mod.direct_superclass()
    end

    return out
  end

  def undef_method(*names)
    names.each do |name|
      name = Type.coerce_to_symbol(name)
      # Will raise a NameError if the method doesn't exist.
      instance_method(name)
      @method_table.store name, nil, :undef
      Rubinius::VM.reset_method_cache(name)

      method_undefined(name) if respond_to? :method_undefined
    end

    nil
  end

  # Like undef_method, but doesn't even check that the method exists. Used
  # mainly to implement rb_undef_method.
  def undef_method!(name)
    name = Type.coerce_to_symbol(name)
    @method_table.store name, nil, :undef
    Rubinius::VM.reset_method_cache(name)

    method_undefined(name) if respond_to? :method_undefined

    name
  end

  def remove_method(*names)
    names.each do |name|
      name = Type.coerce_to_symbol(name)
      # Will raise a NameError if the method doesn't exist.
      instance_method(name)
      unless @method_table.lookup(name)
        raise NameError, "method `#{name}' not defined in #{self.name}"
      end
      @method_table.delete name
      Rubinius::VM.reset_method_cache(name)

      # Use __respond_to_eh__ to avoid hitting unexpected #respod_to?
      # overrides.
      method_removed(name) if __respond_to_eh__ :method_removed
    end

    nil
  end

  def public_method_defined?(sym)
    sym = Type.coerce_to_symbol(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.public? : false
  end

  def private_method_defined?(sym)
    sym = Type.coerce_to_symbol(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.private? : false
  end

  def protected_method_defined?(sym)
    sym = Type.coerce_to_symbol(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.protected? : false
  end

  def method_defined?(sym)
    sym = Type.coerce_to_symbol(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.public? || meth.protected? : false
  end

  ##
  # Returns an UnboundMethod corresponding to the given name. The name will be
  # searched for in this Module as well as any included Modules or
  # superclasses. The UnboundMethod is populated with the method name and the
  # Module that the method was located in.
  #
  # Raises a TypeError if the given name.to_sym fails and a NameError if the
  # name cannot be located.

  def instance_method(name)
    name = Type.coerce_to_symbol name

    mod = self
    while mod
      if entry = mod.method_table.lookup(name)
        break if entry.visibility == :undef

        if meth = entry.method
          if meth.kind_of? Rubinius::Alias
            mod =  meth.original_module
            meth = meth.original_exec
          end

          mod = mod.module if mod.class == Rubinius::IncludedModule

          return UnboundMethod.new(mod, meth, self, name)
        end
      end

      mod = mod.direct_superclass
    end

    raise NameError, "undefined method `#{name}' for #{self}"
  end

  def instance_methods(all=true)
    methods = filter_methods(:public, all) |
              filter_methods(:protected, all)

    Rubinius.convert_to_names methods
  end

  def public_instance_methods(all=true)
    Rubinius.convert_to_names(filter_methods(:public, all))
  end

  def private_instance_methods(all=true)
    Rubinius.convert_to_names(filter_methods(:private, all))
  end

  def protected_instance_methods(all=true)
    Rubinius.convert_to_names(filter_methods(:protected, all))
  end

  def filter_methods(filter, all)
    table = {}

    mod = self

    while mod
      mod.method_table.each do |name, obj, vis|
        unless table.key?(name)
          table[name] = vis
        end
      end

      break unless all
      mod = mod.direct_superclass
    end

    ary = []
    table.each do |name, vis|
      ary << name if vis == filter
    end

    return ary
  end

  private :filter_methods

  def define_method(name, meth = nil, &prc)
    meth ||= prc

    case meth
    when Proc::Method
      cm = Rubinius::DelegatedMethod.new(name, :call, meth, false)
    when Proc
      be = meth.block.dup
      meth = be.method.change_name name.to_sym
      be.method = meth
      cm = Rubinius::BlockEnvironment::AsMethod.new(be)
    when Method
      cm = Rubinius::DelegatedMethod.new(name, :call_on_instance, meth.unbind, true)
    when UnboundMethod
      cm = Rubinius::DelegatedMethod.new(name, :call_on_instance, meth, true)
    else
      raise TypeError, "wrong argument type #{meth.class} (expected Proc/Method)"
    end

    @method_table.store name.to_sym, cm, :public
    Rubinius::VM.reset_method_cache(name.to_sym)

    method_added(name)

    meth
  end

  private :define_method

  def thunk_method(name, value)
    thunk = Rubinius::Thunk.new(value)

    @method_table.store name.to_sym, thunk, :public
    Rubinius::VM.reset_method_cache(name.to_sym)

    method_added(name)

    name
  end

  def extend_object(obj)
    append_features Rubinius.object_metaclass(obj)
  end

  def include?(mod)
    if !mod.kind_of?(Module) or mod.kind_of?(Class)
      raise TypeError, "wrong argument type #{mod.class} (expected Module)"
    end

    return false if self.equal?(mod)

    _each_ancestor { |m| return true if mod.equal?(m) }

    false
  end

  def included_modules
    out = []
    sup = direct_superclass

    while sup
      if sup.class == Rubinius::IncludedModule
        out << sup.module
      end

      sup = sup.direct_superclass
    end

    out
  end

  def set_visibility(meth, vis, where = nil)
    name = Type.coerce_to_symbol(meth)
    vis = vis.to_sym

    if entry = @method_table.lookup(name)
      entry.visibility = vis
    elsif lookup_method(name)
      @method_table.store name, nil, vis
    else
      raise NoMethodError, "Unknown #{where}method '#{name}' to make #{vis.to_s} (#{self})"
    end

    Rubinius::VM.reset_method_cache name

    return name
  end

  def set_class_visibility(meth, vis)
    Rubinius.object_metaclass(self).set_visibility meth, vis, "class "
  end

  def protected(*args)
    if args.empty?
      Rubinius::VariableScope.of_sender.method_visibility = :protected
      return
    end

    args.each { |meth| set_visibility(meth, :protected) }
  end

  def public(*args)
    if args.empty?
      Rubinius::VariableScope.of_sender.method_visibility = nil
      return
    end

    args.each { |meth| set_visibility(meth, :public) }
  end

  def private_class_method(*args)
    args.each do |meth|
      set_class_visibility(meth, :private)
    end
    self
  end

  def public_class_method(*args)
    args.each do |meth|
      set_class_visibility(meth, :public)
    end
    self
  end

  def module_exec(*args, &prc)
    instance_exec(*args, &prc)
  end
  alias_method :class_exec, :module_exec

  def constants
    tbl = Rubinius::LookupTable.new

    @constants.each do |name, val|
      tbl[name] = true
    end

    current = self.direct_superclass

    while current and current != Object
      current.constants_table.each do |name, val|
        tbl[name] = true unless tbl.has_key? name
      end

      current = current.direct_superclass
    end

    # special case: Module.constants returns Object's constants
    if self.equal? Module
      Object.constants_table.each do |name, val|
        tbl[name] = true unless tbl.has_key? name
      end
    end

    Rubinius.convert_to_names tbl.keys
  end

  def const_defined?(name)
    @constants.has_key? normalize_const_name(name)
  end

  # Check if a full constant path is defined, e.g. SomeModule::Something
  def const_path_defined?(name)
    # Start at Object if this is a fully-qualified path
    if name[0,2] == "::" then
      start = Object
      pieces = name[2,(name.length - 2)].split("::")
    else
      start = self
      pieces = name.split("::")
    end

    defined = false
    current = start
    while current and not defined
      const = current
      defined = pieces.all? do |piece|
        if const.is_a?(Module) and const.constants_table.key?(piece)
          const = const.constants_table[piece]
          true
        end
      end
      current = current.direct_superclass
    end
    return defined
  end

  def const_set(name, value)
    if Type.obj_kind_of?(value, Module)
      value.set_name_if_necessary(name, self)
    end

    name = normalize_const_name(name)
    @constants[name] = value
    Rubinius.inc_global_serial

    return value
  end

  ##
  # Return the named constant enclosed in this Module.
  #
  # Included Modules and, for Class objects, superclasses are also searched.
  # Modules will in addition look in Object. The name is attempted to convert
  # using #to_str. If the constant is not found, #const_missing is called
  # with the name.

  def const_get(name)
    recursive_const_get(name)
  end

  def illegal_const(name)
    raise NameError, "constant names must begin with a capital letter: #{name}"
  end

  def const_missing(name)
    raise NameError, "Missing or uninitialized constant: #{self.__name__}::#{name}"
  end

  def <(other)
    unless other.kind_of? Module
      raise TypeError, "compared with non class/module"
    end

    # We're not an ancestor of ourself
    return false if self.equal? other

    _each_ancestor { |mod| return true if mod.equal?(other) }

    nil
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
    Ruby.primitive :module_case_compare
    raise PrimitiveFailure, "Module#=== primitive failed"
  end

  def set_name_if_necessary(name, mod)
    return unless @module_name.nil?
    if mod == Object
      @module_name = name.to_sym
    else
      @module_name = "#{mod.__path__}::#{name}".to_sym
    end
  end

  # Install a new Autoload object into the constants table
  # See kernel/common/autoload.rb
  def autoload(name, path)
    raise TypeError, "autoload filename must be a String" unless path.kind_of? String
    raise ArgumentError, "empty file name" if path.empty?

    return if Rubinius::CodeLoader.feature_provided?(path)

    name = normalize_const_name(name)

    if existing = constant_table[name]
      if existing.kind_of? Autoload
        # If there is already an Autoload here, just change the path to
        # autoload!
        existing.set_path(path)
      else
        # Trying to register an autoload for a constant that already exists,
        # ignore the request entirely.
      end

      return
    end

    constants_table[name] = Autoload.new(name, self, path)
    Rubinius.inc_global_serial
    return nil
  end

  # Is an autoload trigger defined for the given path?
  def autoload?(name)
    name = name.to_sym
    return unless constants_table.key?(name)
    trigger = constants_table[name]
    return unless trigger.kind_of?(Autoload)
    trigger.path
  end

  def remove_const(name)
    unless name.kind_of? Symbol
      name = StringValue name
      illegal_const(name) unless name[0].isupper
    end

    sym = name.to_sym
    unless constants_table.has_key?(sym)
      raise NameError, "Missing or uninitialized constant: #{self.__name__}::#{name}"
    end

    val = constants_table.delete(sym)
    Rubinius.inc_global_serial

    # Silly API compac. Shield Autoload instances
    return nil if val.kind_of? Autoload
    val
  end

  private :remove_const

  def extended(name)
  end

  private :extended

  def method_added(name)
  end

  private :method_added

  # See #const_get for documentation.
  def recursive_const_get(name, missing=true)
    name = normalize_const_name(name)

    current, constant = self, undefined

    while current
      constant = current.constants_table.fetch name, undefined
      unless constant.equal?(undefined)
        constant = constant.call if constant.kind_of?(Autoload)
        return constant
      end

      current = current.direct_superclass
    end

    if instance_of?(Module)
      constant = Object.constants_table.fetch name, undefined
      unless constant.equal?(undefined)
        constant = constant.call if constant.kind_of?(Autoload)
        return constant
      end
    end

    return nil unless missing

    const_missing(name)
  end

  private :recursive_const_get

  def normalize_const_name(name)
    name = Type.coerce_to_symbol(name)

    unless name.is_constant?
      raise NameError, "wrong constant name #{name}"
    end

    name
  end

  private :normalize_const_name

  def initialize_copy(other)
    @method_table = other.method_table.dup
    Rubinius.object_metaclass(self).method_table = Rubinius.object_metaclass(other).method_table.dup

    @constants = Rubinius::LookupTable.new

    other.constants_table.each do |name, val|
      if val.kind_of? Autoload
        val = Autoload.new(val.name, self, val.path)
      end

      @constants[name] = val
    end

    self
  end

  private :initialize_copy

  def add_ivars(cm)
    case cm
    when Rubinius::CompiledMethod
      new_ivars = cm.literals.select { |l| l.kind_of?(Symbol) and l.is_ivar? }
      return if new_ivars.empty?

      if @seen_ivars
        new_ivars.each do |x|
          unless @seen_ivars.include?(x)
            @seen_ivars << x
          end
        end
      else
        @seen_ivars = new_ivars
      end
    when Rubinius::AccessVariable
      if @seen_ivars
        @seen_ivars << cm.name
      else
        @seen_ivars = [cm.name]
      end
    else
      raise "Unknown type of method to learn ivars - #{cm.class}"
    end
  end
end
