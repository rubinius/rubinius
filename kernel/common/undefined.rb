# -*- encoding: us-ascii -*-

class << undefined
  def to_s
    "undefined"
  end

  alias_method :inspect, :to_s
end
