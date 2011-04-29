class Object
  def initialize
  end
  private :initialize

  def __fixnum__
    Ruby.primitive :object_is_fixnum
    kind_of? Fixnum
  end
end
