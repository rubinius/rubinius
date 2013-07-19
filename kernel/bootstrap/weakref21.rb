# -*- encoding: us-ascii -*-

class WeakRef < BasicObject
  def respond_to_missing?(method, include_private)
    target = __getobj__
    target.respond_to?(method, include_private) and
      (!include_private || target.respond_to?(method, false))
  end
end
