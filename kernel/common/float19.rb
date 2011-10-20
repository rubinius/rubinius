class Float
  INFINITY = 1.0 / 0.0
  NAN = 0.0 / 0.0

  def imaginary
    0
  end

  def real?
    true
  end

  def numerator
    if nan?
      NAN
    elsif infinite? == 1
      INFINITY
    elsif infinite? == -1
      -INFINITY
    else
      super
    end
  end

  def denominator
    if infinite? || nan?
      1
    else
      super
    end
  end
end
