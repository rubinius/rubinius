class Selector
  def clear
    Ruby.primitive :selector_clear
    raise PrimitiveFailure, "primitive failed"
  end
end
