class Float < Numeric
  def self.induced_from(o)
    return o.to_f if o.respond_to?(:to_f)
    raise TypeError, "can't convert #{o.class} into Float"
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
    super(other)
  end
  
  def %(other)
    self.divmod(other)[1]
  end
  
  alias :modulo :%

  def round
    Ruby.primitive :float_round
  end
end

