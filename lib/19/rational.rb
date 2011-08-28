# Rational is in the core library in Ruby 1.9.

class Fixnum
  alias quof fdiv
  alias rdiv quo

  alias power! ** unless method_defined? :power!
  alias rpower **
end

class Bignum
  alias quof fdiv
  alias rdiv quo

  alias power! ** unless method_defined? :power!
  alias rpower **
end
