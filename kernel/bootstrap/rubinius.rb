module Rubinius
  def self.watch_signal(sig, ignored)
    Rubinius.primitive :vm_watch_signal
    watch_signal(sig.to_signal, ignored)
  end

  def self.find_method(obj, name)
    Rubinius.primitive :vm_find_method
    raise PrimitiveFailure, "Rubinius.find_method primitive failed"
  end

  def self.find_public_method(obj, name)
    Rubinius.primitive :vm_find_public_method
    raise PrimitiveFailure, "Rubinius.find_public_method primitive failed"
  end

  def self.extended_modules(obj)
    Rubinius.primitive :vm_extended_modules
    raise PrimitiveFailure, "Rubinius.extended_modules primitive failed"
  end

  def self.inc_global_serial
    Rubinius.primitive :vm_inc_global_serial
    raise PrimitiveFailure, "Rubinius.vm_inc_global_serial primitive failed"
  end

  # Deoptimize all methods in the system.
  def self.deoptimize_all(disable)
    Rubinius.primitive :vm_deoptimize_all
    raise PrimitiveFailure, "Rubinius.vm_deoptimize_all primitive failed"
  end

  def self.raise_exception(exc)
    Rubinius.primitive :vm_raise_exception
    raise PrimitiveFailure, "Rubinius.vm_raise_exception primitive failed"
  end

  def self.memory_size(obj)
    Rubinius.primitive :vm_memory_size
    raise PrimitiveFailure, "Rubinius.vm_memory_size primitive failed"
  end

  def self.throw(dest, obj)
    Rubinius.primitive :vm_throw
    raise PrimitiveFailure, "Rubinius.throw primitive failed"
  end

  def self.catch(dest, obj)
    Rubinius.primitive :vm_catch
    raise PrimitiveFailure, "Rubinius.catch primitive failed"
  end

  def self.method_missing_reason
    Rubinius.primitive :vm_method_missing_reason
    :unknown
  end

  def self.constant_missing_reason
    Rubinius.primitive :vm_constant_missing_reason
    :unknown
  end

  def self.mri_backtrace(skip)
    Rubinius.primitive :vm_mri_backtrace
    raise PrimitiveFailure, "Rubinius.mri_backtrace primitive failed"
  end

  def self.add_method(name, executable, mod, vis)
    mod.method_table.store name, executable, :public
    Rubinius::VM.reset_method_cache mod, name
  end

  def self.mathn_loaded?
    false
  end

  module Unsafe
    def self.set_class(obj, cls)
      Rubinius.primitive :vm_set_class

      if obj.kind_of? ImmediateValue
        raise TypeError, "Can not change the class of an immediate"
      end

      raise ArgumentError, "Class #{cls} is not compatible with #{obj.inspect}"
    end
  end

  def self.get_user_home(name)
    Rubinius.primitive :vm_get_user_home
    raise PrimitiveFailure, "Rubinius.get_user_home primitive failed"
  end

  def self.agent_start
    Rubinius.primitive :vm_agent_start
    raise PrimitiveFailure, "Rubinius.agent_start primitive failed"
  end

  def self.agent_loopback
    Rubinius.primitive :vm_agent_loopback
    raise PrimitiveFailure, "Rubinius.agent_loopback primitive failed"
  end

  def self.lock(obj)
    Rubinius.primitive :vm_object_lock
    raise PrimitiveFailure, "Rubinius.lock primitive failed"
  end

  def self.uninterrupted_lock(obj)
    Rubinius.primitive :vm_object_uninterrupted_lock
    raise PrimitiveFailure, "Rubinius.uninterrupted_lock primitive failed"
  end

  def self.lock_timed(obj, duration)
    Rubinius.primitive :vm_object_lock_timed
    raise PrimitiveFailure, "Rubinius.lock primitive failed"
  end

  def self.try_lock(obj)
    Rubinius.primitive :vm_object_trylock
    raise PrimitiveFailure, "Rubinius.try_lock primitive failed"
  end

  def self.locked?(obj)
    Rubinius.primitive :vm_object_locked_p
    raise PrimitiveFailure, "Rubinius.locked? primitive failed"
  end

  def self.unlock(obj)
    Rubinius.primitive :vm_object_unlock
    raise PrimitiveFailure, "Rubinius.unlock primitive failed"
  end

  def self.synchronize(obj)
    lock obj
    begin
      yield
    ensure
      unlock obj
    end
  end

  def self.memory_barrier
    Rubinius.primitive :vm_memory_barrier
    raise PrimitiveFailure, "Rubinius.memory_barrier primitive failed"
  end

  def self.windows?
    Rubinius.primitive :vm_windows_p
    raise PrimitiveFailure, "Rubinius.windows? primitive failed"
  end

  def self.darwin?
    Rubinius.primitive :vm_darwin_p
    raise PrimitiveFailure, "Rubinius.darwin? primitive failed"
  end

  def self.bsd?
    Rubinius.primitive :vm_bsd_p
    raise PrimitiveFailure, "Rubinius.bsd? primitive failed"
  end

  def self.linux?
    Rubinius.primitive :vm_linux_p
    raise PrimitiveFailure, "Rubinius.linux? primitive failed"
  end

  def self.thread_state
    Rubinius.primitive :vm_thread_state
    raise PrimitiveFailure, "Rubinius.thread_state primitive failed"
  end

  def self.check_interrupts
    Rubinius.primitive :vm_check_interrupts
    raise PrimitiveFailure, "Rubinius.check_interrupts primitive failed"
  end

  # Used to invoke a CompiledCode instance as a script body.  Sets up the MAIN
  # object as self and bypasses JIT'ing (because why JIT a script you only run
  # once).
  def self.run_script(compiled_code)
    Rubinius.primitive :vm_run_script
    raise PrimitiveFailure, "Rubinius.run_script primitive failed"
  end

  def self.dtrace_fire(payload)
    Rubinius.primitive :vm_dtrace_fire
    raise PrimitiveFailure, "Rubinius.dtrace_fire primitive failed"
  end

  module Tooling
    def self.raw_load(str)
      Rubinius.primitive :vm_load_tool
      raise PrimitiveFailure, "Tooling.raw_load primitive failed"
    end

    def self.load(str)
      error, reason = raw_load(str)
      unless error == true
        raise ArgumentError, reason
      end

      return true
    end

    def self.available?
      Rubinius.primitive :vm_tooling_available_p
      raise PrimitiveFailure, "Tooling.available? primitive failed"
    end

    def self.active?
      Rubinius.primitive :vm_tooling_active_p
      raise PrimitiveFailure, "Tooling.active? primitive failed"
    end

    def self.enable
      Rubinius.primitive :vm_tooling_enable
      raise PrimitiveFailure, "Tooling.enable primitive failed"
    end

    def self.disable
      Rubinius.primitive :vm_tooling_disable
      raise PrimitiveFailure, "Tooling.disable primitive failed"
    end
  end

  module GC
    def self.count
      Rubinius.primitive :vm_gc_count
      raise PrimitiveFailure, "Rubinius::GC.count primitive failed"
    end

    def self.time
      Rubinius::primitive :vm_gc_time
      raise PrimitiveFailure, "Rubinius::GC.time primitive failed"
    end

    def self.size
      Rubinius::primitive :vm_gc_size
      raise PrimitiveFailure, "Rubinius::GC.size primitive failed"
    end
  end
end
