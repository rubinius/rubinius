# -*- encoding: us-ascii -*-

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

  attr_reader :constant_table
  attr_writer :method_table

  private :included

  def self.nesting
    scope = Rubinius::CompiledCode.of_sender.scope
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

    name = StringValue(name).to_sym
    unless name.is_cvar?
      raise NameError, "#{name} is not an allowed class variable name"
    end
    name
  end
  private :verify_class_variable_name

  def class_variable_set(name, val)
    Rubinius.primitive :module_cvar_set

    class_variable_set verify_class_variable_name(name), val
  end

  def class_variable_get(name)
    Rubinius.primitive :module_cvar_get

    class_variable_get verify_class_variable_name(name)
  end

  def class_variable_defined?(name)
    Rubinius.primitive :module_cvar_defined

    class_variable_defined? verify_class_variable_name(name)
  end

  def remove_class_variable(name)
    Rubinius.primitive :module_cvar_remove

    remove_class_variable verify_class_variable_name(name)
  end

  def __class_variables__
    Rubinius.primitive :module_class_variables

    raise PrimitiveFailure, "Module#__class_variables__ primitive failed"
  end

  def class_variables
    Rubinius::Type.convert_to_names(__class_variables__)
  end

  def name
    Rubinius::Type.module_name self
  end

  def to_s
    Rubinius::Type.module_inspect self
  end

  def inspect
    to_s
  end

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

  def ancestors
    out = []
    Rubinius::Type.each_ancestor(self) { |mod| out << mod }
    return out
  end

  def undef_method(*names)
    names.each do |name|
      # Will raise a NameError if the method doesn't exist.
      instance_method(name)
      undef_method! name
    end

    nil
  end

  # Like undef_method, but doesn't even check that the method exists. Used
  # mainly to implement rb_undef_method.
  def undef_method!(name)
    name = Rubinius::Type.coerce_to_symbol(name)
    @method_table.store name, nil, :undef
    Rubinius::VM.reset_method_cache(name)

    if Rubinius::Type.object_kind_of?(self, Class) and obj = Rubinius::Type.singleton_class_object(self)
      Rubinius.privately do
        obj.singleton_method_undefined(name)
      end
    else
      method_undefined(name)
    end

    name
  end

  def remove_method(*names)
    names.each do |name|
      name = Rubinius::Type.coerce_to_symbol(name)

      unless @method_table.lookup(name)
        raise NameError, "method `#{name}' not defined in #{self.name}"
      end
      @method_table.delete name

      Rubinius::VM.reset_method_cache(name)

      if Rubinius::Type.object_kind_of?(self, Class) and obj = Rubinius::Type.singleton_class_object(self)
        Rubinius.privately do
          obj.singleton_method_removed(name)
        end
      else
        method_removed(name)
      end
    end

    nil
  end

  def public_method_defined?(sym)
    sym = Rubinius::Type.coerce_to_symbol(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.public? : false
  end

  def private_method_defined?(sym)
    sym = Rubinius::Type.coerce_to_symbol(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.private? : false
  end

  def protected_method_defined?(sym)
    sym = Rubinius::Type.coerce_to_symbol(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.protected? : false
  end

  def method_defined?(sym)
    sym = Rubinius::Type.coerce_to_symbol(sym)
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
    name = Rubinius::Type.coerce_to_symbol name

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
    ary = []
    if all and self.direct_superclass
      table = Rubinius::LookupTable.new

      mod = self

      while mod
        mod.method_table.each do |name, obj, vis|
          unless table.key?(name)
            table[name] = true
            ary << name if vis == :public or vis == :protected
          end
        end

        mod = mod.direct_superclass
      end
    else
      method_table.each do |name, obj, vis|
        ary << name if vis == :public or vis == :protected
      end
    end

    Rubinius::Type.convert_to_names ary
  end

  def public_instance_methods(all=true)
    filter_methods(:public, all)
  end

  def private_instance_methods(all=true)
    filter_methods(:private, all)
  end

  def protected_instance_methods(all=true)
    filter_methods(:protected, all)
  end

  def filter_methods(filter, all)
    ary = []
    if all and self.direct_superclass
      table = Rubinius::LookupTable.new
      mod = self

      while mod
        mod.method_table.each do |name, obj, vis|
          unless table.key?(name)
            table[name] = true
            ary << name if vis == filter
          end
        end

        mod = mod.direct_superclass
      end
    else
      method_table.each do |name, obj, vis|
        ary << name if vis == filter
      end
    end

    Rubinius::Type.convert_to_names ary
  end

  private :filter_methods

  def define_method(name, meth = undefined, &prc)
    if meth.equal?(undefined) and !block_given?
      raise ArgumentError, "tried to create Proc object without a block"
    end

    meth = prc if meth.equal?(undefined)

    name = Rubinius::Type.coerce_to_symbol name

    case meth
    when Proc::Method
      code = Rubinius::DelegatedMethod.new(name, :call, meth, false)
    when Proc
      be = meth.block.dup
      be.change_name name
      code = Rubinius::BlockEnvironment::AsMethod.new(be)
      meth = lambda(&meth)
    when Method
      exec = meth.executable
      # We see through delegated methods because code creates these crazy calls
      # to define_method over and over again and if we don't check, we create
      # a huge delegated method chain. So instead, just see through them at one
      # level always.
      if exec.kind_of? Rubinius::DelegatedMethod
        code = exec
      else
        code = Rubinius::DelegatedMethod.new(name, :call_on_instance, meth.unbind, true)
      end
    when UnboundMethod
      exec = meth.executable
      # Same reasoning as above.
      if exec.kind_of? Rubinius::DelegatedMethod
        code = exec
      else
        code = Rubinius::DelegatedMethod.new(name, :call_on_instance, meth, true)
      end
    else
      raise TypeError, "wrong argument type #{meth.class} (expected Proc/Method)"
    end

    Rubinius.add_method name, code, self, :public
    meth
  end

  private :define_method

  def thunk_method(name, value)
    thunk = Rubinius::Thunk.new(value)
    name = Rubinius::Type.coerce_to_symbol name
    Rubinius.add_method name, thunk, self, :public

    name
  end

  def extend_object(obj)
    include_into Rubinius::Type.object_singleton_class(obj)
  end

  def include?(mod)
    if !mod.kind_of?(Module) or mod.kind_of?(Class)
      raise TypeError, "wrong argument type #{mod.class} (expected Module)"
    end

    return false if self.equal?(mod)

    Rubinius::Type.each_ancestor(self) { |m| return true if mod.equal?(m) }

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

  def set_visibility(meth, vis, where=nil)
    name = Rubinius::Type.coerce_to_symbol(meth)
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
    Rubinius::Type.object_singleton_class(self).set_visibility meth, vis, "class "
  end

  def protected(*args)
    if args.empty?
      Rubinius::VariableScope.of_sender.method_visibility = :protected
    else
      args.each { |meth| set_visibility(meth, :protected) }
    end

    self
  end

  def public(*args)
    if args.empty?
      Rubinius::VariableScope.of_sender.method_visibility = nil
    else
      args.each { |meth| set_visibility(meth, :public) }
    end

    self
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
    raise LocalJumpError, "Missing block" unless block_given?

    env = prc.block
    constant_scope = env.repoint_scope self
    return env.call_under(self, constant_scope, *args)
  end
  alias_method :class_exec, :module_exec

  def const_set(name, value)
    name = Rubinius::Type.coerce_to_constant_name name

    if Rubinius::Type.object_kind_of? value, Module
      Rubinius::Type.set_module_name value, name, self
    end

    @constant_table[name] = value
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

  def const_missing(name)
    unless self == Object
      mod_name = "#{Rubinius::Type.module_name self}::"
    end
    raise NameError, "Missing or uninitialized constant: #{mod_name}#{name}"
  end

  def <(other)
    unless other.kind_of? Module
      raise TypeError, "compared with non class/module"
    end

    # We're not an ancestor of ourself
    return false if self.equal? other

    Rubinius::Type.each_ancestor(self) { |mod| return true if mod.equal?(other) }

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
    Rubinius.primitive :module_case_compare
    raise PrimitiveFailure, "Module#=== primitive failed"
  end

  # Is an autoload trigger defined for the given path?
  def autoload?(name)
    name = name.to_sym
    return unless constant_table.key?(name)
    trigger = constant_table[name]
    return unless trigger.kind_of?(Autoload)
    trigger.path
  end

  def remove_const(name)
    unless name.kind_of? Symbol
      name = StringValue name

      unless Rubinius::CType.isupper name.getbyte(0)
        raise NameError, "constant names must begin with a capital letter: #{name}"
      end
    end

    sym = name.to_sym
    unless constant_table.has_key?(sym)
      mod_name = Rubinius::Type.module_name self
      raise NameError, "Missing or uninitialized constant: #{mod_name}::#{name}"
    end

    val = constant_table.delete(sym)
    Rubinius.inc_global_serial

    # Silly API compact. Shield Autoload instances
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

  def method_removed(name)
  end

  private :method_removed

  def method_undefined(name)
  end

  private :method_undefined

  def initialize_copy(other)
    @method_table = other.method_table.dup

    sc_s = Rubinius::Type.object_singleton_class self
    sc_o = Rubinius::Type.object_singleton_class other

    sc_s.method_table = sc_o.method_table.dup
    sc_s.superclass = sc_o.direct_superclass

    @constant_table = Rubinius::LookupTable.new

    other.constant_table.each do |name, val|
      if val.kind_of? Autoload
        val = Autoload.new(val.name, self, val.path)
      end

      @constant_table[name] = val
    end

    self
  end

  private :initialize_copy

  def add_ivars(code)
    case code
    when Rubinius::CompiledCode
      new_ivars = code.literals.select { |l| l.kind_of?(Symbol) and l.is_ivar? }
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
        @seen_ivars << code.name
      else
        @seen_ivars = [code.name]
      end
    else
      raise "Unknown type of method to learn ivars - #{code.class}"
    end
  end

  def dynamic_method(name, file="(dynamic)", line=1)
    g = Rubinius::Generator.new
    g.name = name.to_sym
    g.file = file.to_sym
    g.set_line Integer(line)

    yield g

    g.close

    g.use_detected
    g.encode

    code = g.package Rubinius::CompiledCode

    code.scope =
      Rubinius::ConstantScope.new(self, Rubinius::ConstantScope.new(Object))

    Rubinius.add_method name, code, self, :public

    return code
  end

  def freeze
    @method_table.freeze
    super
  end
end
