# -*- encoding: us-ascii -*-

class Numeric
  def div(other)
    Float(self.__slash__(other)).floor
  end

  def quo(other)
    self.__slash__(other)
  end

  alias_method :fdiv, :quo

  # Delegate #modulo to #% in subclasses
  def modulo(other)
    self % other
  end
end
