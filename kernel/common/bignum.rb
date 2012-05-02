# -*- encoding: us-ascii -*-

class Bignum < Integer

  # see README-DEVELOPERS regarding safe math compiler plugin
  alias_method :/, :divide

  def eql?(value)
    value.is_a?(Bignum) && self == value
  end

  alias_method :modulo, :%

  def fdiv(n)
    to_f / n
  end
end
