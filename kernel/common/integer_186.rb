# These methods are overriden by lib/1.8.7/... or lib/1.9/...
class Integer
  def times
    i = 0
    while i < self
      yield i
      i += 1
    end
    self
  end

  def upto(val)
    i = self
    while i <= val
      yield i
      i += 1
    end
    return self
  end

  def downto(val)
    i = self
    while i >= val
      yield i
      i -= 1
    end
    return self
  end
end