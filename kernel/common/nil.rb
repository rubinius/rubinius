class NilClass
  alias_method :|, :^

  def to_r
    Rational(0, 1)
  end

  def rationalize(eps = nil)
    to_r
  end

  def to_c
    Complex(0)
  end

  def to_h
    {}
  end
end

NIL = nil
