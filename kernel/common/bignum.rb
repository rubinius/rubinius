class Bignum < Integer
  def **(o)
    Rubinius.primitive :bignum_pow
    redo_coerced :**, o
  end

  # see README-DEVELOPERS regarding safe math compiler plugin
  alias_method :/, :divide

  def eql?(value)
    value.is_a?(Bignum) && self == value
  end

  alias_method :modulo, :%

  def fdiv(n)
    to_f / n
  end

  alias_method :quo, :fdiv
end
