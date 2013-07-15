# -*- encoding: us-ascii -*-

class Hash
  def to_h
    instance_of?(Hash) ? self : Hash[self]
  end
end
