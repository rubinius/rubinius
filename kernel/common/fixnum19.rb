class Fixnum < Integer
  def imaginary
    0
  end

  # Must be it's own method, so that super calls the correct method
  # on Numeric
  def div(o)
    Rubinius.primitive :fixnum_div

    if o.is_a?(Float) && o == 0.0
      raise ZeroDivisionError, "division by zero"
    end

    redo_coerced :div, o
  end
end
