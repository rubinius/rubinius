class Rubinius::VM
  # Get an Array of Rubinius::Location objects representing the current
  # call stack. +frames_to_skip+ says how many initial frames to ignore.
  # If +include_vars+ is set, then the Location objects also include a
  # Rubinius::VariableScope object that can be used to access the frames
  # running information like locals, self, etc.
  #
  def self.backtrace(frames_to_skip, *args)
    Rubinius.invoke_primitive :vm_backtrace, Integer(frames_to_skip) + 1
  end

  def self.stats
    Rubinius.primitive :vm_stats
    raise PrimitiveFailure, "Rubinius::VM.stats primitive failed"
  end

  def self.load_library(path, name)
    Rubinius.primitive :load_library
    raise PrimitiveFailure, "Rubinius::VM.load_library primitive failed"
  end

  def self.coerce_to_array(object)
    array = object.respond_to?(:to_a) ? object.to_a : [object]
    raise(TypeError, "`to_a' did not return Array") unless array.is_a?(Array)
    array
  end

  # Semantics of this are very important. ret MUST be returned.
  def self.perform_hook(obj, meth, arg, ret)
    # The VM already verified the method exists
    obj.__send__(meth, arg)
    return ret
  end
end
