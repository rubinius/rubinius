#
#   complex.rb -
#   	$Release Version: 0.5 $
#   	$Revision: 1.3 $
#   	$Date: 1998/07/08 10:05:28 $
#   	by Keiju ISHITSUKA(SHL Japan Inc.)
#
# ----
#
# complex.rb implements the Complex class for complex numbers.  Additionally,
# some methods in other Numeric classes are redefined or added to allow greater
# interoperability with Complex numbers.
#
# Complex numbers can be created in the following manner:
# - <tt>Complex(a, b)</tt>
# - <tt>Complex.polar(radius, theta)</tt>
#
# Additionally, note the following:
# - <tt>Complex::I</tt> (the mathematical constant <i>i</i>)
# - <tt>Numeric#im</tt> (e.g. <tt>5.im -> 0+5i</tt>)
#
# The following +Math+ module methods are redefined to handle Complex arguments.
# They will work as normal with non-Complex arguments.
#    sqrt exp cos sin tan log log10
#    cosh sinh tanh acos asin atan atan2 acosh asinh atanh
#

#
# Creates a Complex number.  +a+ and +b+ should be Numeric.  The result will be
# <tt>a+bi</tt>.
#
def Complex(*args)
  Complex.send :convert, *args
end

#
# The complex number class.
#
class Complex < Numeric
  undef_method :%
  undef_method :<
  undef_method :<=
  undef_method :<=>
  undef_method :>
  undef_method :>=
  undef_method :between?
  undef_method :div
  undef_method :divmod
  undef_method :floor
  undef_method :ceil
  undef_method :modulo
  undef_method :remainder
  undef_method :round
  undef_method :step
  undef_method :truncate
  undef_method :i

  def self.convert(real, imag = undefined)
    if real.equal?(nil) || imag.equal?(nil)
      raise TypeError, "cannot convert nil into Complex"
    end

    real = real.to_c if real.kind_of?(String)
    imag = imag.to_c if imag.kind_of?(String)

    if real.kind_of?(Complex) && !real.imag.kind_of?(Float) && real.imag == 0
      real = real.real
    end

    if imag.kind_of?(Complex) && !imag.imag.kind_of?(Float) && imag.imag == 0
      imag = imag.real
    end

    if real.kind_of?(Complex) && !imag.kind_of?(Float) && imag == 0
      return real
    end

    if imag.equal? undefined
      if real.kind_of?(Numeric) && !real.real?
        return real
      elsif !real.kind_of?(Numeric)
        return Rubinius::Type.coerce_to(real, Complex, :to_c)
      else
        imag = 0
      end
    elsif real.kind_of?(Numeric) && imag.kind_of?(Numeric) && (!real.real? || !imag.real?)
      return real + imag * Complex(0, 1)
    end

    rect(real, imag)
  end

  private_class_method :convert

  def Complex.generic?(other) # :nodoc:
    other.kind_of?(Integer) or
    other.kind_of?(Float) or
    (defined?(Rational) and other.kind_of?(Rational))
  end

  def Complex.rect(real, imag=0)
    raise TypeError, 'not a real' unless check_real?(real) && check_real?(imag)
    new(real, imag)
  end
  class << self; alias_method :rectangular, :rect end

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

  def initialize(a, b = 0)
    @real = a
    @imag = b
  end

  def -@
    Complex(-real, -imag)
  end

  #
  # Addition with real or complex number.
  #
  def +(other)
    if other.kind_of?(Complex)
      Complex(real + other.real, imag + other.imag)
    elsif other.kind_of?(Numeric) && other.real?
      Complex(real + other, imag)
    else
      redo_coerced(:+, other)
    end
  end

  #
  # Subtraction with real or complex number.
  #
  def -(other)
    if other.kind_of?(Complex)
      Complex(real - other.real, imag - other.imag)
    elsif other.kind_of?(Numeric) && other.real?
      Complex(real - other, imag)
    else
      redo_coerced(:-, other)
    end
  end

  #
  # Multiplication with real or complex number.
  #
  def * (other)
    if other.kind_of?(Complex)
      re = @real*other.real - @imag*other.imag
      im = @real*other.imag + @imag*other.real
      Complex(re, im)
    elsif Complex.generic?(other)
      Complex(@real * other, @imag * other)
    else
      x , y = other.coerce(self)
      x * y
    end
  end

  #
  # Division by real or complex number.
  #
  def divide (other)
    if other.kind_of?(Complex)
      self*other.conjugate/other.abs2
    elsif Complex.generic?(other)
      Complex(@real/other, @imag/other)
    else
      x, y = other.coerce(self)
      x/y
    end
  end

  alias_method :/, :divide

  def quo(other)
    Complex(@real.quo(1), @imag.quo(1)) / other
  end

  #
  # Raise this complex number to the given (real or complex) power.
  #
  def ** (other)
    if other == 0
      return Complex(1)
    end
    if other.kind_of?(Complex)
      r, theta = polar
      ore = other.real
      oim = other.imag
      nr = Math.exp(ore*Math.log(r) - oim * theta)
      ntheta = theta*ore + oim*Math.log(r)
      Complex.polar(nr, ntheta)
    elsif other.kind_of?(Integer)
      if other > 0
	x = self
	z = x
	n = other - 1
	while n != 0
	  while (div, mod = n.divmod(2)
		 mod == 0)
	    x = Complex(x.real*x.real - x.imag*x.imag, 2*x.real*x.imag)
	    n = div
	  end
	  z *= x
	  n -= 1
	end
	z
      else
	if defined? Rational
	  (Rational(1) / self) ** -other
	else
	  self ** Float(other)
	end
      end
    elsif Complex.generic?(other)
      r, theta = polar
      Complex.polar(r**other, theta*other)
    else
      x, y = other.coerce(self)
      x**y
    end
  end

  #
  # Absolute value (aka modulus): distance from the zero point on the complex
  # plane.
  #
  def abs
    Math.hypot(@real, @imag)
  end

  alias_method :magnitude, :abs

  #
  # Square of the absolute value.
  #
  def abs2
    @real*@real + @imag*@imag
  end

  #
  # Argument (angle from (1,0) on the complex plane).
  #
  def arg
    Math.atan2(@imag, @real)
  end
  alias angle arg

  #
  # Returns the absolute value _and_ the argument.
  #
  def polar
    [abs, arg]
  end

  #
  # Complex conjugate (<tt>z + z.conjugate = 2 * z.real</tt>).
  #
  def conjugate
    Complex(@real, -@imag)
  end
  alias conj conjugate

  #
  # Test for numerical equality (<tt>a == a + 0<i>i</i></tt>).
  #
  def ==(other)
    if other.kind_of?(Complex)
      real == other.real && imag == other.imag
    elsif other.kind_of?(Numeric) && other.real?
      real == other && imag == 0
    else
      other == self
    end
  end

  #
  # Attempts to coerce +other+ to a Complex number.
  #
  def coerce(other)
    if other.kind_of?(Numeric) && other.real?
      [Complex(other), self]
    elsif other.kind_of?(Complex)
      [other, self]
    else
      raise TypeError, "#{other.class} can't be coerced into Complex"
    end
  end

  def denominator
    @real.denominator.lcm(@imag.denominator)
  end

  def numerator
    cd = denominator
    Complex(@real.numerator*(cd/@real.denominator),
            @imag.numerator*(cd/@imag.denominator))
  end

  def rationalize(eps = nil)
    raise RangeError, "non-zero imaginary part" unless @imag.zero?

    Rational(@real, 1)
  end

  def real?
    false
  end

  def rect
    [@real, @imag]
  end

  alias_method :rectangular, :rect

  def to_f
    raise RangeError, "can't convert #{self} into Float" unless !imag.kind_of?(Float) && imag == 0
    real.to_f
  end

  def to_i
    raise RangeError, "can't convert #{self} into Integer" unless !imag.kind_of?(Float) && imag == 0
    real.to_i
  end

  def to_r
    raise RangeError, "can't' convert #{self} into Rational" unless !imag.kind_of?(Float) && imag == 0
    real.to_r
  end

  #
  # Standard string representation of the complex number.
  #
  def to_s
    signal = "+" unless @imag < 0

    "#{@real}#{signal}#{@imag}i"
  end

  #
  # Returns a hash code for the complex number.
  #
  def hash
    @real.hash ^ @imag.hash
  end

  #
  # Returns "<tt>Complex(<i>real</i>, <i>imag</i>)</tt>".
  #
  def inspect
    "(#{to_s})"
  end

  #
  # Divides each part of a <tt>Complex</tt> number as it is a float.
  #
  def fdiv(other)
    raise TypeError, "#{other.class} can't be coerced into Complex" unless other.is_a?(Numeric)

    # FIXME
    super
  end

  #
  # +I+ is the imaginary number.  It exists at point (0,1) on the complex plane.
  #
  I = Complex(0, 1)

  # The real part of a complex number.
  attr_reader :real

  # The imaginary part of a complex number.
  attr_reader :imag
  alias_method :imaginary, :imag
end
