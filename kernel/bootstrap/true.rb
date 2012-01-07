# -*- encoding: us-ascii -*-

class TrueClass
  def &(other)
    !!other
  end

  def ^(other)
    !other
  end

  def |(other)
    true
  end

  def to_s
    "true"
  end

end

TRUE = true
