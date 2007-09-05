# depends on: integer.rb

class Bignum < Integer
  def %(other)
    self.divmod(other)[1]
  end
  
  alias_method :modulo, :%
  
  alias_method :div, :/
end
