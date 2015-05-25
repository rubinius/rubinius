module Rubinius
  begin
    is_tty = STDIN.tty?
  rescue IOError
    is_tty = false
  end

  Terminal = is_tty
  AtExit = []

  @add_method_hook = Rubinius::Hook.new

  class << self
    attr_reader :add_method_hook
  end

  def self.open_class(name, sup, scope)
    if scope
      under = scope.module
    else
      under = Object
    end

    open_class_under name, sup, under
  end

  def self.open_module_under(name, mod)
    unless Type.object_kind_of? mod, Module
      raise TypeError, "'#{mod.inspect}' is not a class/module"
    end

    tbl = mod.constant_table
    found = tbl.has_name?(name)

    # Object has special behavior, we check it's included
    # modules also
    if !found and mod.equal? Object
      check = mod.direct_superclass

      while check
        tbl = check.constant_table
        found = tbl.has_name?(name)
        break if found
        check = check.direct_superclass
      end
    end

    if !found
      # Create the module
      obj = Module.new
      mod.const_set name, obj
    else
      entry = tbl.lookup(name)
      if entry.visibility == :private
        unless self == Object
          mod_name = "#{Rubinius::Type.module_name mod}::"
        end
        raise NameError, "Private constant: #{mod_name}#{name}"
      end
      obj = entry.constant

      if Type.object_kind_of? obj, Autoload
        obj = obj.call(mod, true)

        # See comment above about autoload returning nil
        unless obj
          obj = Module.new
          mod.const_set name, obj
        end
      end

      if Type.object_kind_of?(obj, Class) || !Type.object_kind_of?(obj, Module)
        raise TypeError, "#{name} is not a module, but a #{obj.class}"
      end
    end
    return obj
  end

  def self.open_module(name, scope)
    if scope
      under = scope.module
    else
      under = Object
    end

    open_module_under name, under
  end

  def self.add_defn_method(name, executable, constant_scope, vis)
    executable.serial = 1
    if executable.respond_to? :scope=
      # If we're adding a method inside ane eval, dup it so that
      # we don't share the CompiledCode with the eval, since
      # we're going to mutate it.
      if constant_scope and script = constant_scope.current_script
        if script.eval?
          executable = executable.dup
        end
      end
      executable.scope = constant_scope
    end

    mod = constant_scope.for_method_definition

    if ai = Type.singleton_class_object(mod)
      if Type.object_kind_of? ai, Numeric

        # Such a weird protocol. If :singleton_method_added exists, allow this.
        # le sigh.
        unless Type.object_respond_to? ai, :singleton_method_added
          raise TypeError, "Unable to define singleton methods on Numerics"
        end
      end
    end

    add_method name, executable, mod, vis
    name
  end

  def self.add_method(name, executable, mod, vis)
    vis ||= :public

    # Don't change the visibility for methods added to singleton
    # classes
    if Type.singleton_class_object(mod)
      visibility = vis
    elsif vis == :module or privatized_method?(name)
      visibility = :private
    else
      visibility = vis
    end

    mod.method_table.store name, executable, visibility
    Rubinius::VM.reset_method_cache mod, name

    Rubinius.privately do
      mod.module_function name if vis == :module
    end

    # Have to use Rubinius::Type.object_respond_to? rather than #respond_to?
    # because code will redefine #respond_to? itself, which is added
    # via #add_method, and then we'll call this new #respond_to?, which
    # commonly can't run yet because it requires methods that haven't been
    # added yet. (ActionMailer does this)

    if obj = Type.singleton_class_object(mod)
      if Type.object_kind_of? obj, Numeric

        # Such a weird protocol. If :singleton_method_added exists, allow this.
        # le sigh.
        unless obj.respond_to? :singleton_method_added
          raise TypeError, "Unable to define singleton methods on Numerics"
        end
      end

      Rubinius.privately do
        obj.singleton_method_added(name)
      end
    else
      case executable
      when CompiledCode, AccessVariable
        mod.add_ivars(executable)
      end

      Rubinius.privately do
        mod.method_added(name)
      end
    end

    @add_method_hook.trigger! mod, name, executable

    return executable
  end

  def self.privatized_method?(name)
    name == :initialize || name == :initialize_copy
  end
  private_class_method :privatized_method?

  # Must be AFTER add_method, because otherwise we'll run this attach_method to add
  # add_method itself and fail.
  def self.attach_method(name, executable, constant_scope, recv)
    executable.serial = 1
    if executable.respond_to? :scope=
      # If we're adding a method inside ane eval, dup it so that
      # we don't share the CompiledCode with the eval, since
      # we're going to mutate it.
      if constant_scope and script = constant_scope.current_script
        if script.eval?
          executable = executable.dup
        end
      end
      executable.scope = constant_scope
    end

    mod = Rubinius::Type.object_singleton_class recv

    add_method name, executable, mod, :public
    name
  end


  def self.add_reader(name, mod, vis)
    normalized = Rubinius::Type.coerce_to_symbol(name)
    add_method normalized, AccessVariable.get_ivar(normalized), mod, vis
  end

  def self.add_writer(name, mod, vis)
    normalized = Rubinius::Type.coerce_to_symbol(name)
    writer_name = "#{normalized}=".to_sym
    add_method writer_name, AccessVariable.set_ivar(normalized), mod, vis
  end

  def self.received_signal(sig)
    Signal.run_handler(sig)
  end

  def self.version
    extra = ""

    if Rubinius::JIT.enabled? and jit = Rubinius::JIT.properties
      extra << "J"

      if jit.include? :inline_generic
        extra << "I"
      end
    end

    extra << "D" if Rubinius::DEBUG_BUILD

    rev = BUILD_REV[0..7]
    llvm = " #{Rubinius::LLVM_VERSION}" if defined?(Rubinius::LLVM_VERSION)

    str = "rubinius #{VERSION} (#{RUBY_VERSION} #{rev} #{RUBY_RELEASE_DATE}#{llvm}"

    unless extra.empty?
      str << " #{extra}"
    end

    str << ") [#{RUBY_PLATFORM}]"

    return str
  end

  def self.unrecognized_defined(file, line)
    message = "#{file}:#{line}: defined? passed an unrecognized expression"
    raise Exception, message
  end

  def self.jump_error(name)
    raise LocalJumpError, "invalid context for '#{name}'"
  end

  # Support for __END__ and DATA
  def self.set_data(name, offset)
    # If the name is nil, then it's not for the main script, so we should
    # ignore it.
    return unless name

    if File.exist? name
      file = File.open name, "r"
      file.seek Integer(offset), IO::SEEK_SET
      Object.const_set :DATA, file
    else
      # Instead of letting the Errno::ENOENT bubble, give a bit more info
      raise "unable to open #{name} to set DATA, source .rb file does not exist."
    end
  end

  def self.compile_file(name)
    ToolSet::Runtime::Compiler.compile name
  end

  def self.allocation_site(obj)
    obj.instance_variable_get("@__allocation_site__")
  end

  ##
  # API Status: official
  #
  # Return the absolute path to the file that contains
  # the current method. This works like __FILE__, but returns
  # the file that require/load resolved directly to, providing
  # an absolute path to the file.
  #
  # Returns nil if there is no file, such as inside eval.
  #
  def self.current_file
    cs = Rubinius::ConstantScope.of_sender
    return cs.absolute_active_path
  end

  def self.privatized_method?(name)
    [:initialize,
     :initialize_copy, :initialize_dup, :initialize_clone,
     :respond_to_missing?].include? name
  end
  private_class_method :privatized_method?

  def self.open_class_under(name, sup, mod)
    unless Type.object_kind_of? mod, Module
      raise TypeError, "'#{mod.inspect}' is not a class/module"
    end

    tbl = mod.constant_table
    if !tbl.has_name?(name)
      # Create the class
      sup = Object unless sup
      obj = Class.new sup, name, mod
    else
      entry = tbl.lookup(name)
      if entry.visibility == :private
        unless self == Object
          mod_name = "#{Rubinius::Type.module_name mod}::"
        end
        raise NameError, "Private constant: #{mod_name}#{name}"
      end
      obj = entry.constant
      if Type.object_kind_of? obj, Autoload
        obj = begin
          obj.call(mod, true)
        rescue NameError # Constant not found in loaded file
          nil
        end

        # nil is returned if the autoload was abort, usually because
        # the file to be required has already been loaded. In which case
        # act like the autoload wasn't there.
        unless obj
          supr = sup ? sup : Object
          obj = Class.new supr, name, mod
        end
      end

      if Type.object_kind_of? obj, Class
        if sup and obj.superclass != sup
          raise TypeError, "Superclass mismatch: #{obj.superclass} != #{sup}"
        end
      else
        raise TypeError, "#{name} is not a class"
      end
    end

    return obj
  end

  # TODO: This is presently duplicated from Kernel::kernel because -> () { }
  # in MRI dispatches to a hidden function that isn't modifiable by overriding
  # Kernel::kernel.
  def self.lambda
    env = nil

    Rubinius.asm do
      push_block
      # assign a pushed block to the above local variable "env"
      # Note that "env" is indexed at 0.
      set_local 0
    end

    raise ArgumentError, "block required" unless env

    prc = Rubinius::Mirror::Proc.from_block ::Proc, env

    # Make a proc lambda only when passed an actual block (ie, not using the
    # "&block" notation), otherwise don't modify it at all.
    prc.lambda_style! if env.is_a?(Rubinius::BlockEnvironment)

    return prc
  end
end
