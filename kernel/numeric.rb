class Integer
  def times
    i = 0
    while i < self
      yield
      i += 1
    end
  end
  
  def upto(val)
    i = self
    while i <= val
      yield i
      i += 1
    end
    return self
  end
end