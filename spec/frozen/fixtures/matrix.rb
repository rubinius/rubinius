class TrivialField < Numeric
  def eql?(rhs)
    rhs.kind_of? TrivialField
  end
end
