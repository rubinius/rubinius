class TrueClass
  def &(other)
    !!other
  end

  def ^(other)
    !other
  end

  def |(other)
    true
  end

  def to_s
    "true"
  end

end

TRUE = true
class TrueClass
  def inspect
    # Call #to_s rather than alias it so that people can change #to_s if they
    # wish. Yes, sounds dumb.
    to_s
  end
end

TRUE = true
