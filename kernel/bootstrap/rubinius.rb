module Rubinius
  def self.watch_signal(sig)
    Ruby.primitive :vm_watch_signal
    watch_signal(sig.to_signal)
  end

  def self.find_method(obj, name)
    Ruby.primitive :vm_find_method
    raise PrimitiveFailure, "Rubinius.find_method failed"
  end

  def self.object_class(obj)
    Ruby.primitive :vm_object_class
    raise PrimitiveFailure, "Rubinius.object_class failed"
  end

  def self.object_metaclass(obj)
    Ruby.primitive :vm_object_metaclass
    raise TypeError, "no metaclass available for a #{obj.class}"
  end

  def self.object_respond_to?(obj, name)
    Ruby.primitive :vm_object_respond_to
    raise PrimitiveFailure, "Rubinius.object_respond_to? failed"
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

  def self.lock(obj)
    Ruby.primitive :vm_object_lock
    raise PrimitiveFailure, "Rubinius.lock failed"
  end

  def self.lock_timed(obj, duration)
    Ruby.primitive :vm_object_lock_timed
    raise PrimitiveFailure, "Rubinius.lock failed"
  end

  def self.try_lock(obj)
    Ruby.primitive :vm_object_trylock
    raise PrimitiveFailure, "Rubinius.try_lock failed"
  end

  def self.locked?(obj)
    Ruby.primitive :vm_object_locked_p
    raise PrimitiveFailure, "Rubinius.locked? failed"
  end

  def self.unlock(obj)
    Ruby.primitive :vm_object_unlock
    raise PrimitiveFailure, "Rubinius.unlock failed"
  end

  def self.memory_barrier
    Ruby.primitive :vm_memory_barrier
    raise PrimitiveFailure, "Rubinius.memory_barrier failed"
  end

  def self.ruby19?
    Ruby.primitive :vm_ruby19_p
    raise PrimitiveFailure, "Rubinius.ruby19? failed"
  end

  def self.ruby20?
    Ruby.primitive :vm_ruby20_p
    raise PrimitiveFailure, "Rubinius.ruby20? failed"
  end

  def self.windows?
    Ruby.primitive :vm_windows_p
    raise PrimitiveFailure, "Rubinius.windows? failed"
  end

  def self.darwin?
    Ruby.primitive :vm_darwin_p
    raise PrimitiveFailure, "Rubinius.darwin? failed"
  end

  def self.bsd?
    Ruby.primitive :vm_bsd_p
    raise PrimitiveFailure, "Rubinius.bsd? failed"
  end

  def self.linux?
    Ruby.primitive :vm_linux_p
    raise PrimitiveFailure, "Rubinius.linux? failed"
  end
end
