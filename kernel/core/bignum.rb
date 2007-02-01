class Bignum < Integer
  def %(other)
    self.divmod(other)[1]
  end
  
  alias :modulo :%
  
  alias :div :/
end
