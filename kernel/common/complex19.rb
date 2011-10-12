class Complex
  #
  # Creates a +Complex+ number in terms of +r+ (radius) and +theta+ (angle).
  #
  def Complex.polar(r, theta=0)
    raise TypeError, 'not a real' unless check_real?(r) && check_real?(theta)

    Complex(r*Math.cos(theta), r*Math.sin(theta))
  end

  def Complex.check_real?(obj)
    obj.kind_of?(Numeric) && obj.real?
  end

  private_class_method :check_real?
end
