class Float < Numeric
  def self.induced_from(obj)
    if [Float, Bignum, Fixnum].include?(obj.class)
      obj.to_f
    else
      raise TypeError, "failed to convert #{obj.class} into Float"
    end
  end

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
  
  def ==(o)
    Ruby.primitive :float_equal
    super(o)
  end

  def eql?(other)
    Ruby.primitive :float_equal
    false
  end
  
  def <=>(other)
    Ruby.primitive :float_compare
    super(other)
  end

  def nan?
    Ruby.primitive :float_nan_p
  end

  def infinite?
    Ruby.primitive :float_infinite_p
  end

  def finite?
    infinite?.nil?
  end

  def to_f
    self
  end
  
  def -@
    Ruby.primitive :float_uminus
  end
  
  def **(other)
    Ruby.primitive :float_pow
    super(other)
  end
  
  def to_i
    Ruby.primitive :float_to_i
  end
  
  alias :to_int :to_i
  alias :truncate :to_i

  def divmod(other)
    Ruby.primitive :float_divmod
    raise FloatDomainError, "divide by 0" if other == 0
    super(other)
  end
  
  def %(other)
    return 0 / 0.to_f if other == 0
    self.divmod(Float(other))[1]
  end
  
  alias :modulo :%

  def round
    Ruby.primitive :float_round
  end
end

