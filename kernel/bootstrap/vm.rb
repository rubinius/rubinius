module VM
  def self.stats
    Ruby.primitive :vm_stats
  end

  def self.load_library(path, name)
    Ruby.primitive :load_library
  end

  def self.reset_method_cache(sym)
    Ruby.primitive :reset_method_cache
  end

  def self.save_encloser_path
    Ruby.primitive :save_encloser_path
  end

  def self.restore_encloser_path
    Ruby.primitive :restore_encloser_path
  end

  def self.coerce_to_array(object)
    array = object.respond_to?(:to_a) ? object.to_a : [object]
    raise TypeError.new("`to_a' did not return Array") unless array.is_a?(Array)
    array
  end

  # Semantics of this are very important. ret MUST be returned.
  def self.perform_hook(obj, meth, arg, ret)
    # The VM already verified the method exists
    obj.__send__(meth, arg)
    return ret
  end
end


class Rubinius::VM
  def self.spawn_prim(args)
    Ruby.primitive :machine_new
  end

  def self.join(id)
    Ruby.primitive :machine_join
  end

  def self.poll_message
    Ruby.primitive :machine_get_message
  end

  def self.send_message(id, obj)
    Ruby.primitive :machine_send_message
  end
end