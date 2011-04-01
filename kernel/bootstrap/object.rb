class Object
  def metaclass
    Rubinius::Type.object_metaclass self
  end
  alias_method :__metaclass__, :metaclass

  def __fixnum__
    Ruby.primitive :object_is_fixnum
    kind_of? Fixnum
  end
end
