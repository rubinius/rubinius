module Rubinius
  Terminal = STDIN.tty?
  AtExit = []

  def self.open_class_under(name, sup, mod)
    unless mod.kind_of? Module
      raise TypeError, "'#{mod.inspect}' is not a class/module"
    end

    tbl = mod.constants_table
    if !tbl.key?(name)
      # Create the class
      sup = Object unless sup
      obj = Class.new sup, name, mod
    else
      obj = tbl[name]
      if obj.kind_of? Autoload
        obj = obj.call
      end

      if obj.kind_of? Class
        if sup and obj.superclass != sup
          raise TypeError, "Superclass mismatch: #{obj.superclass} != #{sup}"
        end
      else
        raise TypeError, "#{name} is not a class"
      end
    end
    return obj
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
    unless mod.kind_of? Module
      raise TypeError, "'#{mod.inspect}' is not a class/module"
    end

    tbl = mod.constants_table
    if !tbl.key?(name)
      # Create the module
      obj = Module.new
      obj.set_name_if_necessary name, mod
      mod.const_set name, obj
    else
      obj = tbl[name]
      if obj.kind_of? Autoload
        obj = obj.call
      end

      unless obj.kind_of? Module
        raise TypeError, "#{name} is not a class"
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

  def self.add_defn_method(name, executable, static_scope, vis)
    executable.serial = 1
    executable.scope = static_scope if executable.respond_to? :scope=

    mod = static_scope.for_method_definition

    if mod.kind_of? Class and ai = mod.__metaclass_object__
      if ai.kind_of? Numeric

        # Such a weird protocol. If :singleton_method_added exists, allow this.
        # le sigh.
        unless ai.respond_to? :singleton_method_added
          raise TypeError, "Unable to define singleton methods on Numerics"
        end
      end
    end

    add_method name, executable, mod, vis
  end

  def self.add_method(name, executable, mod, vis)
    vis ||= :public

    if vis == :module or name == :initialize or name == :initialize_copy
      visibility = :private
    else
      visibility = vis
    end

    if entry = mod.method_table.lookup(name)
      Rubinius.deoptimize_inliners entry.method if entry.method
    end

    mod.method_table.store name, executable, visibility
    Rubinius::VM.reset_method_cache(name)

    mod.module_function name if vis == :module

    # Have to use Rubinius.object_respond_to? rather than #respond_to?
    # because code will redefine #respond_to? itself, which is added
    # via #add_method, and then we'll call this new #respond_to?, which
    # commonly can't run yet because it requires methods that haven't been
    # added yet. (ActionMailer does this)

    if mod.kind_of? Class and obj = mod.__metaclass_object__
      if obj.kind_of? Numeric

        # Such a weird protocol. If :singleton_method_added exists, allow this.
        # le sigh.
        unless obj.respond_to? :singleton_method_added
          raise TypeError, "Unable to define singleton methods on Numerics"
        end
      end

      if Rubinius.object_respond_to? obj, :singleton_method_added
        obj.singleton_method_added(name)
      end
    else
      if Rubinius.object_respond_to? mod, :method_added
        mod.method_added(name)
      end
    end

    return executable
  end

  # Must be AFTER add_method, because otherwise we'll run this attach_method to add
  # add_method itself and fail.
  def self.attach_method(name, executable, static_scope, recv)
    executable.serial = 1
    executable.scope = static_scope if executable.respond_to? :scope=

    mod = Rubinius.object_metaclass recv

    add_method name, executable, mod, :public
  end


  def self.add_reader(name, mod, vis)
    normalized = Type.coerce_to_symbol(name)
    add_method normalized, AccessVariable.get_ivar(normalized), mod, vis
  end

  def self.add_writer(name, mod, vis)
    normalized = Type.coerce_to_symbol(name)
    writer_name = "#{normalized}=".to_sym
    add_method writer_name, AccessVariable.set_ivar(normalized), mod, vis
  end

  def self.received_signal(sig)
    Signal.run_handler(sig)
  end

  def self.jit(meth)
    cm = meth.executable

    return unless cm.respond_to? :jit_now

    unless status = cm.jit_now
      puts "AOT: unable to compile #{meth}"
    end

    return status
  end

  def self.jit_soon(meth)
    cm = meth.executable
    cm.jit_soon
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
    if File.exists? name
      file = File.open name, "r"
      file.seek Integer(offset), IO::SEEK_SET
      Object.const_set :DATA, file
    else
      # Instead of letting the Errno::ENOENT bubble, give a bit more info
      raise "unable to open #{name} to set DATA, source .rb file does not exist."
    end
  end
end

# A wierd place for it, but it works.
RUBY_DESCRIPTION = Rubinius.version
