class Object
  def __fixnum__
    Rubinius.primitive :object_is_fixnum
    kind_of? Fixnum
  end
end
