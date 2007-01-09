class Float < Numeric
  # include Precision
  
  def to_s
    Ruby.primitive :float_to_s
  end
  
  alias :inspect :to_s

  def +(o)
    Ruby.primitive :float_add
  end

  def -(o)
    Ruby.primitive :float_sub
  end

  def *(o)
    Ruby.primitive :float_mul
  end

  def ==(o)
    Ruby.primitive :float_equal
  end
end
