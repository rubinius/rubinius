class Object
  def singleton_class
    Rubinius::Type.object_singleton_class self
  end

  def __fixnum__
    Ruby.primitive :object_is_fixnum
    kind_of? Fixnum
  end
end
