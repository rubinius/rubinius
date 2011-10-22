#
# Creates a Complex number.  +a+ and +b+ should be Numeric.  The result will be
# <tt>a+bi</tt>.
#
def Complex(a, b = 0)
  if b == 0
    a
  else
    Complex.new(a.real - b.imag, a.imag + b.real)
  end
end
