class Integer
  def times
    i = 0
    while i < self
      yield
      i += 1
    end
  end
end