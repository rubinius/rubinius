module Kernel
  alias_method :send, :__send__

  def to_a
    if self.kind_of? Array
      self
    else
      [self]
    end
  end

  alias_method :proc, :lambda
  module_function :proc
end
