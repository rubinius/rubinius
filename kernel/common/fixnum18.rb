# -*- encoding: us-ascii -*-

class Fixnum < Integer
  # Must be it's own method, so that super calls the correct method
  # on Numeric
  def div(o)
    divide(o).floor
  end

  alias_method :quo, :fdiv

  def **(o)
    Rubinius.primitive :fixnum_pow
    redo_coerced :**, o
  end
end
