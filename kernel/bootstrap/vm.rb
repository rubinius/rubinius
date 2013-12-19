class Rubinius::VM

  # Get an Array of Rubinius::Location objects representing the current
  # call stack. +frames_to_skip+ says how many initial frames to ignore.
  # If +include_vars+ is set, then the Location objects also include a
  # Rubinius::VariableScope object that can be used to access the frames
  # running information like locals, self, etc.
  #
  def self.backtrace(frames_to_skip, include_vars=false)
    Rubinius.primitive :vm_backtrace

    # Add the + 1 to skip this frame
    backtrace(Integer(frames_to_skip) + 1, include_vars)
  end

  def self.stats
    Rubinius.primitive :vm_stats
    raise PrimitiveFailure, "Rubinius::VM.stats primitive failed"
  end

  def self.dump_heap(path)
    Rubinius.primitive :vm_dump_heap
    raise PrimitiveFailure, "Rubinius::VM.dump_heap primitive failed"
  end

  def self.__jit_info__
    Rubinius.primitive :vm_jit_info
    raise PrimitiveFailure, "Rubinius::VM.jit_info primitive failed"
  end

  class JITInfo
    def initialize(methods, bytes, nanoseconds, accessors_inlined, uncommons)
      @jitted_methods = methods
      @code_bytes = bytes
      @nanoseconds = nanoseconds
      @accessors_inlined = accessors_inlined
      @uncommons = uncommons
    end

    attr_reader :jitted_methods
    attr_reader :code_bytes
    attr_reader :nanoseconds
    attr_reader :accessors_inlined
    attr_reader :uncommons

    def milliseconds
      @nanoseconds / 1_000_000
    end

    def seconds
      @nanoseconds.to_f / 1_000_000_000
    end

    def bytes_per_method
      @code_bytes.to_f / @jitted_methods
    end
  end

  def self.jit_info
    JITInfo.new(*__jit_info__)
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
