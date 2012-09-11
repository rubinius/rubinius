# -*- encoding: us-ascii -*-

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
    found = tbl.key?(name)

    # Object has special behavior, we check it's included
    # modules also
    if !found and mod.equal? Object
      check = mod.direct_superclass

      while check
        tbl = check.constant_table
        found = tbl.key?(name)
        break if found
        check = check.direct_superclass
      end
    end

    if !found
      # Create the module
      obj = Module.new
      mod.const_set name, obj
    else
      obj = tbl[name]
      if Type.object_kind_of? obj, Autoload
        obj = obj.call(mod, true)

        # See comment above about autoload returning nil
        unless obj
          obj = Module.new
          mod.const_set name, obj
        end
      end

      if Type.object_kind_of?(obj, Class) || !Type.object_kind_of?(obj, Module)
        raise TypeError, "#{name} is not a module"
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

    if Type.object_kind_of?(mod, Class) and ai = Type.singleton_class_object(mod)
      if Type.object_kind_of? ai, Numeric

        # Such a weird protocol. If :singleton_method_added exists, allow this.
        # le sigh.
        unless Type.object_respond_to? ai, :singleton_method_added
          raise TypeError, "Unable to define singleton methods on Numerics"
        end
      end
    end

    add_method name, executable, mod, vis
  end

  def self.add_method(name, executable, mod, vis)
    vis ||= :public

    # Don't change the visibility for methods added to singleton
    # classes
    if Type.object_kind_of?(mod, Class) and Type.singleton_class_object(mod)
      visibility = vis
    elsif vis == :module or name == :initialize or name == :initialize_copy
      visibility = :private
    else
      visibility = vis
    end

    if entry = mod.method_table.lookup(name)
      Rubinius.deoptimize_inliners entry.method if entry.method
    end

    mod.method_table.store name, executable, visibility
    Rubinius::VM.reset_method_cache(name)

    Rubinius.privately do
      mod.module_function name if vis == :module
    end

    # Have to use Rubinius::Type.object_respond_to? rather than #respond_to?
    # because code will redefine #respond_to? itself, which is added
    # via #add_method, and then we'll call this new #respond_to?, which
    # commonly can't run yet because it requires methods that haven't been
    # added yet. (ActionMailer does this)

    if Type.object_kind_of?(mod, Class) and obj = Type.singleton_class_object(mod)
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

    if jit = Rubinius::JIT
      extra << "J"

      if jit.include? :inline_generic
        extra << "I"
      end
    end

    str = "rubinius #{VERSION} (#{RUBY_VERSION} #{BUILD_REV[0..7]} #{RUBY_RELEASE_DATE}"

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

    if File.exists? name
      file = File.open name, "r"
      file.seek Integer(offset), IO::SEEK_SET
      Object.const_set :DATA, file
    else
      # Instead of letting the Errno::ENOENT bubble, give a bit more info
      raise "unable to open #{name} to set DATA, source .rb file does not exist."
    end
  end

  def self.compile_file(name)
    Compiler.compile name
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

  def self.terminal_width
    return @terminal_width if @terminal_width
    if Terminal and ENV['TERM'] and !ENV['RBX_NO_COLS']
      if ENV["COLUMNS"]
        @terminal_width = ENV["COLUMNS"].to_i
      end
      begin
        `which stty &> /dev/null`
        if $?.exitstatus == 0
          @terminal_width = `stty size`.split.last.to_i
        end
      end
      begin
        `which tput &> /dev/null`
        if $?.exitstatus == 0
          @terminal_width = `tput cols`.to_i
        end
      end
    end
    @terminal_width = 80 if @terminal_width.nil? || @terminal_width.zero?
    @terminal_width
  end

end
