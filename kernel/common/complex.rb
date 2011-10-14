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
# Numeric is a built-in class on which Fixnum, Bignum, etc., are based.  Here
# some methods are added so that all number types can be treated to some extent
# as Complex numbers.
#
class Numeric
  #
  # Returns a Complex number <tt>(0,<i>self</i>)</tt>.
  #
  def im
    Complex(0, self)
  end
  
  #
  # The real part of a complex number, i.e. <i>self</i>.
  #
  def real
    self
  end
  
  #
  # The imaginary part of a complex number, i.e. 0.
  #
  def image
    0
  end
  alias imag image
  
  #
  # See Complex#arg.
  #
  def arg
    Math.atan2(0, self)
  end
  alias angle arg
  alias phase arg
  
  #
  # See Complex#polar.
  #
  def polar
    return abs, arg
  end
  
  #
  # See Complex#conjugate (short answer: returns <i>self</i>).
  #
  def conjugate
    self
  end
  alias conj conjugate
end


#
# Creates a Complex number.  +a+ and +b+ should be Numeric.  The result will be
# <tt>a+bi</tt>.
#
def Complex(a, b = 0)
  if b == 0 and (a.kind_of?(Complex) or defined? Complex::Unify)
    a
  else
    Complex.new( a.real-b.imag, a.imag+b.real )
  end
end

#
# The complex number class.  See complex.rb for an overview.
#
class Complex < Numeric
  @RCS_ID='-$Id: complex.rb,v 1.3 1998/07/08 10:05:28 keiju Exp keiju $-'

  undef_method :step
  undef_method :div
  undef_method :divmod
  undef_method :floor
  undef_method :truncate
  undef_method :ceil
  undef_method :round

  def Complex.generic?(other) # :nodoc:
    other.kind_of?(Integer) or
    other.kind_of?(Float) or
    (defined?(Rational) and other.kind_of?(Rational))
  end

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


  #
  # Creates a +Complex+ number <tt>a</tt>+<tt>b</tt><i>i</i>.
  #
  def Complex.new!(a, b=0)
    new(a,b)
  end

  def initialize(a, b)
    raise TypeError, "non numeric 1st arg `#{a.inspect}'" if !a.kind_of? Numeric
    raise TypeError, "`#{a.inspect}' for 1st arg" if a.kind_of? Complex
    raise TypeError, "non numeric 2nd arg `#{b.inspect}'" if !b.kind_of? Numeric
    raise TypeError, "`#{b.inspect}' for 2nd arg" if b.kind_of? Complex
    @real = a
    @image = b
  end

  #
  # Addition with real or complex number.
  #
  def + (other)
    if other.kind_of?(Complex)
      re = @real + other.real
      im = @image + other.image
      Complex(re, im)
    elsif Complex.generic?(other)
      Complex(@real + other, @image)
    else
      x , y = other.coerce(self)
      x + y
    end
  end
  
  #
  # Subtraction with real or complex number.
  #
  def - (other)
    if other.kind_of?(Complex)
      re = @real - other.real
      im = @image - other.image
      Complex(re, im)
    elsif Complex.generic?(other)
      Complex(@real - other, @image)
    else
      x , y = other.coerce(self)
      x - y
    end
  end
  
  #
  # Multiplication with real or complex number.
  #
  def * (other)
    if other.kind_of?(Complex)
      re = @real*other.real - @image*other.image
      im = @real*other.image + @image*other.real
      Complex(re, im)
    elsif Complex.generic?(other)
      Complex(@real * other, @image * other)
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
      Complex(@real/other, @image/other)
    else
      x, y = other.coerce(self)
      x/y
    end
  end

  alias_method :/, :divide
  
  def quo(other)
    Complex(@real.quo(1), @image.quo(1)) / other
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
      oim = other.image
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
	    x = Complex(x.real*x.real - x.image*x.image, 2*x.real*x.image)
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
  # Remainder after division by a real or complex number.
  #
  def % (other)
    if other.kind_of?(Complex)
      Complex(@real % other.real, @image % other.image)
    elsif Complex.generic?(other)
      Complex(@real % other, @image % other)
    else
      x , y = other.coerce(self)
      x % y
    end
  end
  
#--
#    def divmod(other)
#      if other.kind_of?(Complex)
#        rdiv, rmod = @real.divmod(other.real)
#        idiv, imod = @image.divmod(other.image)
#        return Complex(rdiv, idiv), Complex(rmod, rmod)
#      elsif Complex.generic?(other)
#        Complex(@real.divmod(other), @image.divmod(other))
#      else
#        x , y = other.coerce(self)
#        x.divmod(y)
#      end
#    end
#++
  
  #
  # Absolute value (aka modulus): distance from the zero point on the complex
  # plane.
  #
  def abs
    Math.hypot(@real, @image)
  end
  
  alias_method :magnitude, :abs
  
  #
  # Square of the absolute value.
  #
  def abs2
    @real*@real + @image*@image
  end
  
  #
  # Argument (angle from (1,0) on the complex plane).
  #
  def arg
    Math.atan2(@image, @real)
  end
  alias angle arg
  
  #
  # Returns the absolute value _and_ the argument.
  #
  def polar
    return abs, arg
  end
  
  #
  # Complex conjugate (<tt>z + z.conjugate = 2 * z.real</tt>).
  #
  def conjugate
    Complex(@real, -@image)
  end
  alias conj conjugate
  
  #
  # Compares the absolute values of the two numbers.
  #
  def <=> (other)
    self.abs <=> other.abs
  end
  
  #
  # Test for numerical equality (<tt>a == a + 0<i>i</i></tt>).
  #
  def == (other)
    if other.kind_of?(Complex)
      @real == other.real and @image == other.image
    elsif Complex.generic?(other)
      @real == other and @image == 0
    else
      other == self
    end
  end

  #
  # Attempts to coerce +other+ to a Complex number.
  #
  def coerce(other)
    if Complex.generic?(other)
      return Complex.new!(other), self
    else
      super
    end
  end

  #
  # FIXME
  #
  def denominator
    @real.denominator.lcm(@image.denominator)
  end
  
  #
  # FIXME
  #
  def numerator
    cd = denominator
    Complex(@real.numerator*(cd/@real.denominator),
	    @image.numerator*(cd/@image.denominator))
  end
  
  def rationalize
    raise RangeError, "non-zero imaginary part" unless @image.zero?
    
    Rational(@real,1)
  end

  def real?
    false
  end
  
  def rect
    return @real, @image
  end
  
  alias_method :rectangular, :rect
  
  #
  # Standard string representation of the complex number.
  #
  def to_s
    signal = "+" unless @image < 0
    
    "#{@real}#{signal}#{@image}i"
  end
  
  #
  # Returns a hash code for the complex number.
  #
  def hash
    @real.hash ^ @image.hash
  end
  
  #
  # Returns "<tt>Complex(<i>real</i>, <i>image</i>)</tt>".
  #
  def inspect
    "(#{to_s})"
  end

  
  #
  # +I+ is the imaginary number.  It exists at point (0,1) on the complex plane.
  #
  I = Complex(0,1)
  
  # The real part of a complex number.
  attr_reader :real

  # The imaginary part of a complex number.
  attr_reader :image
  alias imag image
  alias imaginary image
  
end

class Integer

  unless defined?(1.numerator)
    def numerator() self end
    def denominator() 1 end

    def gcd(other)
      min = self.abs
      max = other.abs
      while min > 0
        tmp = min
        min = max % min
        max = tmp
      end
      max
    end

    def lcm(other)
      if self.zero? or other.zero?
        0
      else
        (self.div(self.gcd(other)) * other).abs
      end
    end

  end

end
class Complex
end
