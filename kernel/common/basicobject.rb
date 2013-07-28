# -*- encoding: us-ascii -*-

class BasicObject
  def method_missing(meth, *args)
    ::Kernel.raise ::NoMethodError, "Unable to send '#{meth}' on instance of BasicObject"
  end
  private :method_missing

  def singleton_method_added(name) end
  private :singleton_method_added

  def singleton_method_removed(name) end
  private :singleton_method_removed

  def singleton_method_undefined(name) end
  private :singleton_method_undefined

  def __all_instance_variables__
    Rubinius.primitive :object_ivar_names

    raise PrimitiveFailure, "BasicObject#__instance_variables__ failed"
  end
  private :__all_instance_variables__

  def __instance_variables__
    ary = []
    __all_instance_variables__.each do |sym|
      ary << sym if sym.is_ivar?
    end

    Rubinius::Type.convert_to_names ary
  end
end
