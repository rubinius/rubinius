# -*- encoding: us-ascii -*-

class Exception
  alias_method :to_str, :message

  def to_s
    @reason_message || self.class.to_s
  end
end
