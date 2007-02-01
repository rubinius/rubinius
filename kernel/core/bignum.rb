class Bignum < Integer
  def %(other)
    self.divmod(other)[1]
  end
  
  alias :modulo :%
  
  def quo(other)
    Float(self) / Float(other)
  end
  
  alias :div :/
end
