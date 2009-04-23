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
end
