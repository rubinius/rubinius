class Rational
  def to_s
    @numerator.to_s+"/"+@denominator.to_s
  end
  
  def inspect
    "(#{to_s})"
  end
end