class Integer
  # Returns true if int is an even number. 
  def even?
    self[0].zero?
  end

  # Returns true if int is an odd number. 
  def odd?
    !even?
  end

  # Returns the int itself.
  def ord
    self
  end

  # Returns the Integer equal to int - 1.
  def pred
    self - 1
  end
  
  def times
    return to_enum :times unless block_given?
    i = 0
    while i < self
      yield i
      i += 1
    end
    self
  end

  def upto(val)
    return to_enum :upto, val unless block_given?
    i = self
    while i <= val
      yield i
      i += 1
    end
    self
  end

  def downto(val)
    return to_enum :downto, val unless block_given?
    i = self
    while i >= val
      yield i
      i -= 1
    end
    self
  end
end