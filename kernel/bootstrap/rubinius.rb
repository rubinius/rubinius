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
end
