# -*- encoding: us-ascii -*-

module Rubinius
  def self.watch_signal(sig, ignored)
    Rubinius.primitive :vm_watch_signal
    watch_signal(sig.to_signal, ignored)
  end

  def self.find_method(obj, name)
    Rubinius.primitive :vm_find_method
    raise PrimitiveFailure, "Rubinius.find_method failed"
  end

  def self.find_public_method(obj, name)
    Rubinius.primitive :vm_find_public_method
    raise PrimitiveFailure, "Rubinius.find_public_method failed"
  end

  def self.extended_modules(obj)
    Rubinius.primitive :vm_extended_modules
    raise PrimitiveFailure, "Rubinius.extended_modules failed"
  end

  def self.inc_global_serial
    Rubinius.primitive :vm_inc_global_serial
    raise PrimitiveFailure, "Rubinius.vm_inc_global_serial failed"
  end

  def self.jit_block(be, show)
    Rubinius.primitive :vm_jit_block
    raise PrimitiveFailure, "Rubinius.vm_jit_block failed"
  end

  def self.deoptimize_inliners(exec)
    Rubinius.primitive :vm_deoptimize_inliners
    raise PrimitiveFailure, "Rubinius.vm_deoptimize_inliners failed"
  end

  # Deoptimize all methods in the system.
  def self.deoptimize_all(disable)
    Rubinius.primitive :vm_deoptimize_all
    raise PrimitiveFailure, "Rubinius.vm_deoptimize_all failed"
  end

  def self.raise_exception(exc)
    Rubinius.primitive :vm_raise_exception
    raise PrimitiveFailure, "Rubinius.vm_raise_exception failed"
  end

  def self.memory_size(obj)
    Rubinius.primitive :vm_memory_size
    raise PrimitiveFailure, "Rubinius.vm_memory_size failed"
  end

  def self.throw(dest, obj)
    Rubinius.primitive :vm_throw
    raise PrimitiveFailure, "Rubinius.throw failed"
  end

  def self.catch(dest, obj)
    Rubinius.primitive :vm_catch
    raise PrimitiveFailure, "Rubinius.catch failed"
  end

  def self.method_missing_reason
    Rubinius.primitive :vm_method_missing_reason
    :unknown
  end

  def self.mri_backtrace(skip)
    Rubinius.primitive :vm_mri_backtrace
    raise PrimitiveFailure, "Rubinius.mri_backtrace failed"
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

  def self.kcode=(str)
    Rubinius.primitive :vm_set_kcode
    raise PrimitiveFailure, "Rubinius.kcode= failed"
  end

  def self.kcode
    Rubinius.primitive :vm_get_kcode
    raise PrimitiveFailure, "Rubinius.kcode failed"
  end

  def self.get_user_home(name)
    Rubinius.primitive :vm_get_user_home
    raise PrimitiveFailure, "Rubinius.get_user_home failed"
  end

  def self.agent_io
    Rubinius.primitive :vm_agent_io
    raise PrimitiveFailure, "Rubinius.agent_io failed"
  end

  def self.lock(obj)
    Rubinius.primitive :vm_object_lock
    raise PrimitiveFailure, "Rubinius.lock failed"
  end

  def self.lock_timed(obj, duration)
    Rubinius.primitive :vm_object_lock_timed
    raise PrimitiveFailure, "Rubinius.lock failed"
  end

  def self.try_lock(obj)
    Rubinius.primitive :vm_object_trylock
    raise PrimitiveFailure, "Rubinius.try_lock failed"
  end

  def self.locked?(obj)
    Rubinius.primitive :vm_object_locked_p
    raise PrimitiveFailure, "Rubinius.locked? failed"
  end

  def self.unlock(obj)
    Rubinius.primitive :vm_object_unlock
    raise PrimitiveFailure, "Rubinius.unlock failed"
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
    raise PrimitiveFailure, "Rubinius.memory_barrier failed"
  end

  def self.ruby18?
    Rubinius.primitive :vm_ruby18_p
    raise PrimitiveFailure, "Rubinius.ruby18? failed"
  end

  def self.ruby19?
    Rubinius.primitive :vm_ruby19_p
    raise PrimitiveFailure, "Rubinius.ruby19? failed"
  end

  def self.ruby20?
    Rubinius.primitive :vm_ruby20_p
    raise PrimitiveFailure, "Rubinius.ruby20? failed"
  end

  def self.windows?
    Rubinius.primitive :vm_windows_p
    raise PrimitiveFailure, "Rubinius.windows? failed"
  end

  def self.darwin?
    Rubinius.primitive :vm_darwin_p
    raise PrimitiveFailure, "Rubinius.darwin? failed"
  end

  def self.bsd?
    Rubinius.primitive :vm_bsd_p
    raise PrimitiveFailure, "Rubinius.bsd? failed"
  end

  def self.linux?
    Rubinius.primitive :vm_linux_p
    raise PrimitiveFailure, "Rubinius.linux? failed"
  end

  def self.thread_state
    Rubinius.primitive :vm_thread_state
    raise PrimitiveFailure, "Rubinius.thread_state failed"
  end

  # Used to invoke a CompiledMethod +cm+ as a script body.
  # Sets up the MAIN object as self and bypasses JIT'ing
  # (because why JIT a script you only run once).
  def self.run_script(cm)
    Rubinius.primitive :vm_run_script
    raise PrimitiveFailure, "Rubinius.run_script failed"
  end

  module Tooling
    def self.raw_load(str)
      Rubinius.primitive :vm_load_tool
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
      Rubinius.primitive :vm_tooling_available_p
      raise PrimitiveFailure, "Tooling.available? failed"
    end

    def self.active?
      Rubinius.primitive :vm_tooling_active_p
      raise PrimitiveFailure, "Tooling.active? failed"
    end

    def self.enable
      Rubinius.primitive :vm_tooling_enable
      raise PrimitiveFailure, "Tooling.enable failed"
    end

    def self.disable
      Rubinius.primitive :vm_tooling_disable
      raise PrimitiveFailure, "Tooling.disable failed"
    end
  end
end
