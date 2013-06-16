# -*- encoding: us-ascii -*-

class WeakRef

  class RefError < ::RuntimeError; end

  def self.new(obj)
    Rubinius.primitive :weakref_new
    ::Kernel.raise PrimitiveFailure, "WeakRef.new primitive failed"
  end

  def __setobj__(obj)
    Rubinius.primitive :weakref_set_object
    ::Kernel.raise PrimitiveFailure, "WeakRef#__setobj__ primitive failed"
  end

  def __object__
    Rubinius.primitive :weakref_object
    ::Kernel.raise PrimitiveFailure, "WeakRef#__object__ primitive failed"
  end

  def __getobj__
    obj = __object__()
    ::Kernel.raise RefError, "Object has been collected as garbage" unless obj
    return obj
  end

  def weakref_alive?
    !!__object__
  end

  def method_missing(method, *args, &block)
    target = __getobj__
    if target.respond_to?(method)
      target.__send__(method, *args, &block)
    else
      super(method, *args, &block)
    end
  end
end
