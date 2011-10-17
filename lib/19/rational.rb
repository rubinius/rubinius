# :enddoc:

class Rational < Numeric
  def div(other)
    if other.is_a?(Float) && other == 0.0
      raise ZeroDivisionError, "division by zero"
    end

    (self / other).floor
  end
end

class Fixnum

  alias quof fdiv
  alias rdiv quo

  alias power! ** unless method_defined? :power!
  alias rpower **

end

class Bignum

  alias quof fdiv
  alias rdiv quo

  alias power! ** unless method_defined? :power!
  alias rpower **

end
