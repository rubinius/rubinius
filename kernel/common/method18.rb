# -*- encoding: us-ascii -*-

class Method
  def name
    @name.to_s
  end

  ##
  # Method objects are equal if they have the same body and are bound to the
  # same object.

  def ==(other)
    other.class == Method and
      @receiver.equal?(other.receiver) and
      @executable == other.executable
  end
end

class UnboundMethod

  def name
    @name.to_s
  end

end

