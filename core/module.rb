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
    scope = Rubinius::LexicalScope.of_sender
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

  private :initialize

  def deprecate_constant(*syms)
    # no-op for now
  end

  def const_get(name, inherit=true)
    Rubinius::Type.const_get self, name, inherit
  end

  def const_defined?(name, inherit=true)
    c = Rubinius::Type.const_get self, name, inherit, false
    return c.equal?(undefined) ? false : true
  end

  def attr(*attributes)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    if attributes.size == 2 && (attributes.last.is_a?(TrueClass) || attributes.last.is_a?(FalseClass))
      name = attributes.first
      Rubinius.add_reader name, self, vis
      Rubinius.add_writer name, self, vis if attributes.last
    else
      attributes.each do |name|
        if name.is_a?(Symbol) || (name.respond_to?(:to_str) && name.to_str.is_a?(String) && !name.to_str.empty?)
          Rubinius.add_reader(name, self, vis)
        else
          raise TypeError, "#{name.inspect} is not a symbol"
        end
      end
    end

    return nil
  end

  private :attr

  # Install a new Autoload object into the constants table
  # See core/autoload.rb
  def autoload(name, path)
    path = Rubinius::Type.coerce_to_path(path)

    raise ArgumentError, "empty file name" if path.empty?

    name = Rubinius::Type.coerce_to_constant_name name

    Rubinius.check_frozen

    if entry = @constant_table.lookup(name)
      if entry.constant.kind_of? Autoload
        # If there is already an Autoload here, just change the path to
        # autoload!
        entry.constant.set_path(path)
      else
        # Trying to register an autoload for a constant that already exists,
        # ignore the request entirely.
      end

      return
    end

    autoload_constant = Autoload.new(name, self, path)
    constant_table.store(name, autoload_constant, :public)
    if self == Kernel
      Object.singleton_class.constant_table.store(name, autoload_constant, :public)
    end

    Rubinius.inc_global_serial
    return nil
  end

  def constants(all=undefined)
    tbl = Rubinius::LookupTable.new

    @constant_table.each do |name, constant, visibility|
      tbl[name] = true unless visibility == :private
    end

    if all
      current = self.direct_superclass

      while current and current != Object
        current.constant_table.each do |name, constant, visibility|
          tbl[name] = true unless visibility == :private
        end

        current = current.direct_superclass
      end
    end

    # special case: Module.constants returns Object's constants
    if self.equal?(Module) && undefined.equal?(all)
      Object.constant_table.each do |name, constant, visibility|
        tbl[name] = true unless visibility == :private
      end
    end

    tbl.keys
  end

  #--
  # These have to be aliases, not methods that call instance eval, because we
  # need to pull in the binding of the person that calls them, not the
  # intermediate binding.
  #++

  def module_eval(string=undefined, filename="(eval)", line=1, &prc)
    # we have a custom version with the prc, rather than using instance_exec
    # so that we can setup the LexicalScope properly.
    if prc
      unless undefined.equal?(string)
        raise ArgumentError, "cannot pass both string and proc"
      end

      # Return a copy of the BlockEnvironment with the receiver set to self
      env = prc.block
      lexical_scope = env.repoint_scope self
      return env.call_under(self, lexical_scope, true, self)
    elsif undefined.equal?(string)
      raise ArgumentError, 'block not supplied'
    end

    string = StringValue(string)
    filename = StringValue(filename)

    # The constantscope of a module_eval CM is the receiver of module_eval
    cs = Rubinius::LexicalScope.new self, Rubinius::LexicalScope.of_sender

    binding = Binding.setup(Rubinius::VariableScope.of_sender,
                            Rubinius::CompiledCode.of_sender,
                            cs)

    c = Rubinius::ToolSets::Runtime::Compiler
    be = c.construct_block string, binding, filename, line

    be.call_under self, cs, true, self
  end

  alias_method :class_eval, :module_eval

  def private_constant(*names)
    names = names.map(&:to_sym)
    unknown_constants = names - @constant_table.keys
    if unknown_constants.size > 0
      raise NameError, "#{unknown_constants.size > 1 ? 'Constants' : 'Constant'} #{unknown_constants.map{|e| "#{name}::#{e}"}.join(', ')} undefined"
    end
    names.each do |name|
      entry = @constant_table.lookup(name)
      entry.visibility = :private
    end
  end

  def public_constant(*names)
    names = names.map(&:to_sym)
    unknown_constants = names - @constant_table.keys
    if unknown_constants.size > 0
      raise NameError, "#{unknown_constants.size > 1 ? 'Constants' : 'Constant'} #{unknown_constants.map{|e| "#{name}::#{e}"}.join(', ')} undefined"
    end
    names.each do |name|
      entry = @constant_table.lookup(name)
      entry.visibility = :public
    end
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
      if mod == mod.origin && entry = mod.method_table.lookup(name)
        break if entry.visibility == :undef

        if meth = entry.get_method
          if meth.kind_of? Rubinius::Alias
            meth = meth.original_exec
          end

          mod = mod.module if mod.class == Rubinius::IncludedModule

          return UnboundMethod.new(mod, meth, self, name)
        end
      end

      mod = mod.direct_superclass
    end

    raise NameError.new("undefined method `#{name}' for #{self}", name)
  end

  def public_instance_method(name)
    name = Rubinius::Type.coerce_to_symbol name

    mod = self
    while mod
      if mod == mod.origin && entry = mod.method_table.lookup(name)
        vis = entry.visibility
        break if vis == :undef

        if meth = entry.method
          if meth.kind_of? Rubinius::Alias
            meth = meth.original_exec
          end

          mod = mod.module if mod.class == Rubinius::IncludedModule

          if vis == :public
            return UnboundMethod.new(mod, meth, self, name)
          else
            raise NameError.new("method `#{name}' for module `#{self}' is #{vis}", name)
          end
        end
      end

      mod = mod.direct_superclass
    end

    raise NameError.new("undefined method `#{name}' for #{self}", name)
  end

  def visibility_for_aliased_method(new_name, current_name_visibility)
    special_methods = [
      :initialize,
      :initialize_copy,
      :initialize_clone,
      :initialize_dup,
      :respond_to_missing?
    ]

    if special_methods.include?(new_name)
      :private
    else
      current_name_visibility
    end
  end
  private :visibility_for_aliased_method

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
  alias_method :class_variables, :__class_variables__

  def lookup_method(sym, check_object_too=true, trim_im=true)
    mod = self

    while mod
      if mod == mod.origin && entry = mod.method_table.lookup(sym.to_sym)
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

  def public_method_defined?(sym)
    sym = Rubinius::Type.coerce_to_reflection_name(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.public? : false
  end

  def private_method_defined?(sym)
    sym = Rubinius::Type.coerce_to_reflection_name(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.private? : false
  end

  def protected_method_defined?(sym)
    sym = Rubinius::Type.coerce_to_reflection_name(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.protected? : false
  end

  def method_defined?(sym)
    sym = Rubinius::Type.coerce_to_symbol(sym)
    mod, meth = lookup_method(sym, false)
    meth ? meth.public? || meth.protected? : false
  end

  def instance_methods(all=true)
    ary = []
    if all
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

    ary
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

    ary
  end

  private :filter_methods

  def define_method(name, meth = undefined, &prc)
    if undefined.equal?(meth) and !block_given?
      raise ArgumentError, "tried to create Proc object without a block"
    end

    meth = prc if undefined.equal?(meth)

    name = Rubinius::Type.coerce_to_symbol name

    if meth.respond_to?(:for_define_method)
      code, scope = meth.for_define_method(name, self.class)
    else
      raise TypeError, "wrong argument type #{meth.class} (expected Proc/Method)"
    end

    Rubinius.add_method name, code, self, scope, 0, :public

    name
  end

  private :define_method

  def thunk_method(name, value)
    thunk = Rubinius::Thunk.new(value)
    name = Rubinius::Type.coerce_to_symbol name
    Rubinius.add_method name, thunk, self, nil, 0, :public

    name
  end

  def extend_object(obj)
    include_into Rubinius::Type.object_singleton_class(obj)
  end
  private :extend_object

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

    Rubinius::VM.reset_method_cache self, name

    vis = vis.to_sym

    if entry = @method_table.lookup(name)
      entry.visibility = vis
    elsif lookup_method(name)
      @method_table.store name, nil, nil, nil, 0, vis
    else
      raise NameError.new("Unknown #{where}method '#{name}' to make #{vis.to_s} (#{self})", name)
    end

    return name
  end

  def set_class_visibility(meth, vis)
    Rubinius::Type.object_singleton_class(self).set_visibility meth, vis, "class "
  end
  private :set_class_visibility

  def module_exec(*args, &prc)
    raise LocalJumpError, "Missing block" unless block_given?

    env = prc.block
    lexical_scope = env.repoint_scope self
    return env.call_under(self, lexical_scope, true, *args)
  end
  alias_method :class_exec, :module_exec

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
    case Rubinius.constant_missing_reason
    when :private
      msg = "Private constant: #{mod_name}#{name}"
    else
      msg = "Missing or uninitialized constant: #{mod_name}#{name}"
    end

    raise NameError.new(msg, name)
  end

  def <(other)
    unless other.kind_of? Module
      raise TypeError, "compared with non class/module"
    end

    # We're not an ancestor of ourself
    return false if self.equal? other

    Rubinius::Type.each_ancestor(self) { |mod| return true if mod.equal?(other) }
    Rubinius::Type.each_ancestor(other) { |mod| return false if mod.equal?(self) }

    nil
  end

  def <=(other)
    equal?(other) || (self < other)
  end

  def >(other)
    unless other.kind_of? Module
      raise TypeError, "compared with non class/module"
    end
    other < self
  end

  def >=(other)
    equal?(other) || (self > other)
  end

  def <=>(other)
    return 0 if self.equal? other
    return nil unless other.kind_of? Module

    lt = self < other
    return nil if lt.nil?
    lt ? -1 : 1
  end

  def ===(inst)
    Rubinius::Type.object_kind_of?(inst, self)
  end

  # Is an autoload trigger defined for the given path?
  def autoload?(name)
    name = name.to_sym
    entry = constant_table.lookup(name)
    return unless entry
    trigger = entry.constant
    return unless trigger && trigger.kind_of?(Autoload)
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
    unless constant_table.has_name?(sym)
      mod_name = Rubinius::Type.module_name self
      raise NameError, "Missing or uninitialized constant: #{mod_name}::#{name}"
    end

    val = constant_table.delete(sym)
    Rubinius.inc_global_serial

    # Silly API compact. Shield Autoload instances
    return nil if Rubinius::Type.object_kind_of?(val, Autoload)
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
    # If the method table is already different, we already
    # initialized this module.
    unless @method_table == other.method_table
      raise TypeError, "already initialized module"
    end

    @method_table = other.method_table.dup

    sc_s = Rubinius::Type.object_singleton_class self
    sc_o = Rubinius::Type.object_singleton_class other

    sc_s.method_table = sc_o.method_table.dup
    sc_s.superclass = sc_o.direct_superclass

    @constant_table = Rubinius::ConstantTable.new

    other.constant_table.each do |name, val|
      if val.kind_of? Autoload
        val = Autoload.new(val.name, self, val.path)
      end

      @constant_table.store(name, val, :public)
    end

    if other == other.origin
      @origin = self
    else
      @origin = other.origin.dup
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
        unless @seen_ivars.include?(code.name)
          @seen_ivars << code.name
        end
      else
        @seen_ivars = [code.name]
      end
    else
      raise "Unknown type of method to learn ivars - #{code.class}"
    end
  end

  def dynamic_method(name, file="(dynamic)", line=1)
    g = Rubinius::ToolSets::Runtime::Generator.new
    g.name = name.to_sym
    g.file = file.to_sym
    g.set_line Integer(line)

    yield g

    g.close

    g.use_detected
    g.encode

    code = g.package Rubinius::CompiledCode

    scope = Rubinius::LexicalScope.new(self, Rubinius::LexicalScope.new(Object))
    code.scope = scope

    Rubinius.add_method name, code, self, scope, 0, :public

    return code
  end

  def freeze
    @method_table.freeze
    @constant_table.freeze
    super
  end

  def origin= origin
    @origin = origin
  end

  def prepended(mod); end
  private :prepended

  def prepend(*modules)
    modules.reverse_each do |mod|
      if !mod.kind_of?(Module) or mod.kind_of?(Class)
        raise TypeError, "wrong argument type #{mod.class} (expected Module)"
      end

      Rubinius.privately do
        mod.prepend_features self
      end

      Rubinius.privately do
        mod.prepended self
      end
    end
    self
  end

  def prepend_features(klass)
    unless klass.kind_of? Module
      raise TypeError, "invalid argument class #{klass.class}, expected Module"
    end

    if kind_of? Class
      raise TypeError, "invalid receiver class #{__class__}, expected Module"
    end

    if klass == klass.origin
      im = Rubinius::IncludedModule.new(klass)
      im.superclass = klass.direct_superclass
      klass.superclass = im
      klass.origin = im
      klass.method_table.each do |meth, obj, vis|
        Rubinius::VM.reset_method_cache klass, meth
      end
    end
    Rubinius::Type.include_modules_from(self, klass)
    Rubinius::Type.infect(klass, self)
    self
  end
  private :prepend_features

  def singleton_class?
    !!Rubinius::Type.singleton_class_object(self)
  end
end
