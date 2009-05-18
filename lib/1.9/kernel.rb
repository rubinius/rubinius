module Kernel
  alias_method :__callee__, :__method__
  
  def define_singleton_method(*args, &block)
    class << self
      self
    end.send(:define_method, *args, &block)
  end
end