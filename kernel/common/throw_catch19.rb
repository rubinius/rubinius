# -*- encoding: us-ascii -*-

module Kernel
  def catch(obj = Object.new, &block)
    raise LocalJumpError unless block_given?

    Rubinius::ThrownValue.register(obj) do
      return Rubinius.catch(obj, block)
    end
  end
  module_function :catch

  def throw(obj, value=nil)
    unless Rubinius::ThrownValue.available? obj
      raise ArgumentError, "uncaught throw #{obj.inspect}"
    end

    Rubinius.throw obj, value
  end
  module_function :throw
end
