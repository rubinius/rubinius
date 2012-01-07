# -*- encoding: us-ascii -*-

class Integer
  alias_method :round, :to_i

  def chr
    if self > 255 || self < 0
      raise RangeError, "#{self} is out of the valid character range"
    end
    String.pattern 1, self
  end
end
