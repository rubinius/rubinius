class Numeric
  def i
    Complex(0, self)
  end
  
  def rationalize
    Rational(self,1)
  end
end
