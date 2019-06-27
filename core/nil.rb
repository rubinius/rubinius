class NilClass
  alias_method :eql?, :equal?
  alias_method :==,  :equal?
  alias_method :===, :equal?

  def &(other)
    false
  end

  def ^(other)
    !!other
  end

  alias_method :|, :^

  def to_s
    ""
  end

  def inspect
    "nil"
  end

  def to_a
    []
  end

  def to_f
    0.0
  end

  def to_i
    0
  end

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
