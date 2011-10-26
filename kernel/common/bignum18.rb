class Bignum < Integer
  alias_method :quo, :fdiv
end
