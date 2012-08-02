# -*- encoding: us-ascii -*-

class Float
  def **(other)
    Rubinius.primitive :float_pow
    b, a = math_coerce other
    a ** b
  end

  def round
    Rubinius.primitive :float_round
    raise PrimitiveFailure, "Float#round primitive failed"
  end
end
