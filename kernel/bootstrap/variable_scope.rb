class VariableScope
  def self.of_sender
    Ruby.primitive :variable_scope_of_sender

    raise PrimitiveFailure, "Unable to get VariableScope of sender"
  end

  attr_accessor :last_match
  ##
  # The Nth group of the last regexp match.

  def nth_ref(n)
    if lm = @last_match
      return lm[n]
    end

    return nil
  end

  ##
  # One of the special globals $&, $`, $' or $+.

  def back_ref(kind)
    if lm = @last_match
      res = case kind
      when :&
        lm[0]
      when :"`"
        lm.pre_match
      when :"'"
        lm.post_match
      when :+
        lm.captures.last
      end

      return res
    end

    return nil
  end
end
