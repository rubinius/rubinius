class FalseClass
  def &(other)
    false
  end

  def ^(other)
    !!other
  end

  def to_s
    "false"
  end
end

FALSE = false
