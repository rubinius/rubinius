class NilClass
  def &(other)
    false
  end

  def ^(other)
    !!other
  end

  def to_s
    ""
  end

  def inspect
    "nil"
  end

  def nil?
    true
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
end
