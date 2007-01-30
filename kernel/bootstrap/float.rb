class Float < Numeric
  
  def to_s
    Ruby.primitive :float_to_s
  end
  
  alias :inspect :to_s

  def +(other)
    Ruby.primitive :float_add
    super(other)
  end

  def -(other)
    Ruby.primitive :float_sub
    super(other)
  end

  def *(other)
    Ruby.primitive :float_mul
    super(other)
  end

  def /(other)
    Ruby.primitive :float_div
    super(other)
  end
  
  def ==(other)
    Ruby.primitive :float_equal
    super(other)
  end
  
  def <=>(other)
    Ruby.primitive :float_compare
    super(other)
  end
end
