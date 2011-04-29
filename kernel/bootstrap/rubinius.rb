module Rubinius
  # Ruby 1.8 returns strings for method and constant names
  def self.convert_to_names(list)
    list.map! { |x| x.to_s }
  end

  def self.watch_signal(sig)
    Ruby.primitive :vm_watch_signal
    watch_signal(sig.to_signal)
  end

  def self.find_method(obj, name)
    Ruby.primitive :vm_find_method
    raise PrimitiveFailure, "Rubinius.find_method failed"
  end

  def self.extended_modules(obj)
    Ruby.primitive :vm_extended_modules
    raise PrimitiveFailure, "Rubinius.extended_modules failed"
  end

  def self.inc_global_serial
    Ruby.primitive :vm_inc_global_serial
    raise PrimitiveFailure, "Rubinius.vm_inc_global_serial failed"
  end

  def self.jit_block(be, show)
    Ruby.primitive :vm_jit_block
    raise PrimitiveFailure, "Rubinius.vm_jit_block failed"
  end

  def self.deoptimize_inliners(exec)
    Ruby.primitive :vm_deoptimize_inliners
    raise PrimitiveFailure, "Rubinius.vm_deoptimize_inliners failed"
  end

  # Deoptimize all methods in the system.
  def self.deoptimize_all(disable)
    Ruby.primitive :vm_deoptimize_all
    raise PrimitiveFailure, "Rubinius.vm_deoptimize_all failed"
  end

  def self.raise_exception(exc)
    Ruby.primitive :vm_raise_exception
    raise PrimitiveFailure, "Rubinius.vm_raise_exception failed"
  end

  def self.memory_size(obj)
    Ruby.primitive :vm_memory_size
    raise PrimitiveFailure, "Rubinius.vm_memory_size failed"
  end

  def self.throw(dest, obj)
    Ruby.primitive :vm_throw
    raise PrimitiveFailure, "Rubinius.throw failed"
  end

  def self.catch(dest, obj)
    Ruby.primitive :vm_catch
    raise PrimitiveFailure, "Rubinius.catch failed"
  end

  def self.method_missing_reason
    Ruby.primitive :vm_method_missing_reason
    :unknown
  end

  def self.mri_backtrace(skip)
    Ruby.primitive :vm_mri_backtrace
    raise PrimitiveFailure, "Rubinius.mri_backtrace failed"
  end

  module Unsafe
    def self.set_class(obj, cls)
      Ruby.primitive :vm_set_class

      if obj.kind_of? ImmediateValue
        raise TypeError, "Can not change the class of an immediate"
      end

      raise ArgumentError, "Class #{cls} is not compatible with #{obj.inspect}"
    end
  end

  def self.kcode=(str)
    Ruby.primitive :vm_set_kcode
    raise PrimitiveFailure, "Rubinius.kcode= failed"
  end

  def self.kcode
    Ruby.primitive :vm_get_kcode
    raise PrimitiveFailure, "Rubinius.kcode failed"
  end

  def self.get_user_home(name)
    Ruby.primitive :vm_get_user_home
    raise PrimitiveFailure, "Rubinius.get_user_home failed"
  end

  def self.agent_io
    Ruby.primitive :vm_agent_io
    raise PrimitiveFailure, "Rubinius.agent_io failed"
  end

  def self.thread_state
    Ruby.primitive :vm_thread_state
    raise PrimitiveFailure, "Rubinius.thread_state failed"
  end

  # Used to invoke a CompiledMethod +cm+ as a script body.
  # Sets up the MAIN object as self and bypasses JIT'ing
  # (because why JIT a script you only run once).
  def self.run_script(cm)
    Ruby.primitive :vm_run_script
    raise PrimitiveFailure, "Rubinius.run_script failed"
  end

  module Tooling
    def self.raw_load(str)
      Ruby.primitive :vm_load_tool
      raise PrimitiveFailure, "Tooling.raw_load failed"
    end

    def self.load(str)
      error, reason = raw_load(str)
      unless error == true
        raise ArgumentError, reason
      end

      return true
    end

    def self.available?
      Ruby.primitive :vm_tooling_available_p
      raise PrimitiveFailure, "Tooling.available? failed"
    end

    def self.active?
      Ruby.primitive :vm_tooling_active_p
      raise PrimitiveFailure, "Tooling.active? failed"
    end

    def self.enable
      Ruby.primitive :vm_tooling_enable
      raise PrimitiveFailure, "Tooling.enable failed"
    end

    def self.disable
      Ruby.primitive :vm_tooling_disable
      raise PrimitiveFailure, "Tooling.disable failed"
    end
  end
end
