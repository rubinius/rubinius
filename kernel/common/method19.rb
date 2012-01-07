# -*- encoding: us-ascii -*-

class Method
  def name
    @name
  end

  alias_method :eql?, :==
end

class UnboundMethod

  def name
    @name
  end

end

