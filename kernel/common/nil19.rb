class NilClass
  def to_r
    Rational(0, 1)
  end

  def rationalize(eps = nil)
    to_r
  end
end
