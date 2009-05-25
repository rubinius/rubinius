module Kernel
  alias_method :__callee__, :__method__

  def define_singleton_method(*args, &block)
    class << self
      self
    end.send(:define_method, *args, &block)
  end

  def StringValue(obj)
    return obj.to_s if obj.is_a? Symbol # 1.9 treats symbols as string-like
    Type.coerce_to(obj, String, :to_str)
  end
end

module Rubinius
  # No need to convert symbols to strings anymore
  def self.convert_to_names(list)
    list
  end
end