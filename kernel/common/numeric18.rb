class Numeric
  def div(other)
    Float(self.__slash__(other)).floor
  end

  # Delegate #modulo to #% in subclasses
  def modulo(other)
    self % other
  end
end
