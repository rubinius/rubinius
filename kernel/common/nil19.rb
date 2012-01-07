# -*- encoding: us-ascii -*-

class NilClass
  def to_r
    Rational(0, 1)
  end

  def rationalize(eps = nil)
    to_r
  end

  def to_c
    Complex(0)
  end
end
