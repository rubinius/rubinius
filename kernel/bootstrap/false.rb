class FalseClass
  def &(other)
    false
  end

  def ^(other)
    (other.equal?(false) or other.nil?) ? false : true
  end

  def to_s
    "false"
  end
end

FALSE = false
