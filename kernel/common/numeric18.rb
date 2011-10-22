class Numeric
  def div(other)
    Float(self.__slash__(other)).floor
  end
end
