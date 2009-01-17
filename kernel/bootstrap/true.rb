class TrueClass
  def &(other)
    (other.nil? or other.equal?(false)) ? false : true
  end

  def ^(other)
    (other.nil? or other.equal?(false)) ? true : false
  end

  def |(other)
    true
  end

  def to_s
    "true"
  end

end

TRUE = true
