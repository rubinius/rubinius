# -*- encoding: us-ascii -*-

class Bignum < Integer
  alias_method :quo, :fdiv

  def **(o)
    Rubinius.primitive :bignum_pow
    redo_coerced :**, o
  end
end
