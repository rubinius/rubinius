class Object
  def metaclass
    class << self;self;end
  end
  alias_method :__metaclass__, :metaclass

  def __fixnum__
    Ruby.primitive :object_is_fixnum
    kind_of? Fixnum
  end
end
