class Float
  def **(other)
    Rubinius.primitive :float_pow
    b, a = math_coerce other
    a ** b
  end
end
