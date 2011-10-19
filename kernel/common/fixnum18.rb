class Fixnum < Integer
  # Must be it's own method, so that super calls the correct method
  # on Numeric
  def div(o)
    Rubinius.primitive :fixnum_div
    redo_coerced :div, o
  end
end
